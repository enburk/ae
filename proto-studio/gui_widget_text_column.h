#pragma once
#include "gui_widgetarium.h"
#include "gui_widget_text.h"
namespace gui::text
{
    struct token final : widgetarium<glyph>, metrics
    {
        struct data { str text; style_index style;
            bool operator != (const data & d) const { return !(*this == d); }
            bool operator == (const data & d) const { return true
                && text  == d.text
                && style == d.style;
            }
        };

        static inline std::unordered_map<int, int> spaces;

        str text; style_index style; void fill (
        str text, style_index style)
        {
            if (this->text == text && this->style == style) return;
                this->text =  text;   this->style =  style;

            width = ascent = descent = advance = 0;

            glyphs = unicode::glyphs(text);
            reserve(glyphs.size());

            for (int i=0; i<glyphs.size(); i++)
            {
                glyph & glyph = (*this)(i);
                glyph.value = sys::glyph(glyphs[i], style);
                ascent  = max(ascent,  glyph.value.now.ascent);
                descent = max(descent, glyph.value.now.descent);
            }
            truncate(glyphs.size());
            for (auto & glyph : *this)
            {
                glyph.move_to(XY(width + advance, ascent - glyph.value.now.ascent));
                width  += advance + glyph.value.now.width;
                advance = glyph.value.now.advance;
            }
            resize(XY(width, ascent + descent));
        }
        array<str> glyphs;
    };

    ///////////////////////////////////////////////////////////////////////

    struct row : metrics
    {
        format format;
        array<int> offsets;
        array<int> indices;
        bool the_last_row = true;

        bool add (array<token*> tokens, int index)
        {
            int Width =
                format.width -
                format.margin_left.x -
                format.margin_right.x;

            int tokens_width = 0;
            int tokens_advance = 0;
            for (auto token : tokens) {
                tokens_width += token->width + token->advance;
                tokens_advance = token->advance;
            }
            tokens_width -= tokens_advance;

            if (format.word_wrap
            &&  Width < width + advance + tokens_width
            &&  offsets.size() > 0) // at least one should be accepted
            {
                the_last_row = false;
                return false;
            }

            for (auto token : tokens)
            {
                indices += index;
                offsets += width + advance;
                width = offsets.back() + token->width;
                advance = token->advance;
                ascent  = max (ascent,  token->ascent);
                descent = max (descent, token->descent);
                outlines.size.y = ascent + descent;
                outlines.size.x = token->text == " " || token->text == "\n" ?
                outlines.size.x : width;
            }
            return true;
        }

        void align ()
        {
            int align = format.alignment.x;
            int width = outlines.size.x;
            int Width =
                format.width -
                format.margin_left.x -
                format.margin_right.x;

            outlines.x = format.margin_left.x;

            if (align == left || align == justify_left && the_last_row) {
                return;
            }

            if (align == center && Width > width) {
                outlines.x += Width/2 - width/2;
                return;
            }

            if (align == right || align == justify_right && the_last_row) {
                outlines.x += Width - width;
                return;
            }

            outlines.size.x = Width;

            int n = offsets.size();
            if (n < 1) return;
            if (n < 2) return; //  t o k e n

            int d = (Width - width) / (n-1);
            int e = (Width - width) % (n-1);

            for (int i=0; i<n; i++) offsets[i] += d*i + (i >= n-e ? 1 : 0);
        }
    };

    ///////////////////////////////////////////////////////////////////////
            
    struct line final : widgetarium<token>
    {
        struct data { format format; array<token::data> tokens;
            bool operator != (const data & d) const { return !(*this == d); }
            bool operator == (const data & d) const { return true
                && format == d.format
                && tokens == d.tokens;
            }
        };

        data data_copy;
        array<row> rows;

        void fill (data data)
        {
            data_copy = std::move(data);
            auto & format = data_copy.format;
            auto & tokens = data_copy.tokens;
            reserve(tokens.size());

            rows.clear();

            if (format.width  <= 0) { resize(XY()); return; }
            if (format.height <= 0) { resize(XY()); return; }

            if (format.width == max<int>() && format.alignment.x != left)
                throw std::out_of_range("text::line: format impossible");

            int total = 0; int height = 0;

            array<token*> unbreakable;

            for(auto [lexeme, n] : tokens.whole())
            {
                token & token = (*this)(n);
                token.fill(lexeme.text, lexeme.style);

                unbreakable += &token;
                if (token.text != " " &&
                    token.text != "\n" &&
                    n != tokens.size()-1)
                    continue;

                if (rows.size() == 0 || !rows.back().add(unbreakable, total))
                {
                    if (rows.size() != 0)
                    {
                        int h =                         
                        rows.back().ascent +
                        rows.back().descent;
                        if (height + h >= format.height) break;
                        height += h;

                        format.height         = max (0, format.height         - h);
                        format.margin_left.y  = max (0, format.margin_left.y  - h);
                        format.margin_right.y = max (0, format.margin_right.y - h);
                    }

                    rows += row{};
                    rows.back().format = format;
                    rows.back().outlines.y = height;
                    rows.back().add(unbreakable, total);
                    // at least one glyph would be accepted
                    // and no row would be empty
                }

                total +=
                unbreakable.size();
                unbreakable.clear();
            }

            if (rows.size() != 0) height +=
                rows.back().ascent +
                rows.back().descent;

            truncate(total);

            int t = 0;

            int width = 0;

            for (auto & row : rows)
            {
                row.align();

                for (auto offset : row.offsets)
                {
                    token & token = (*this)(t++);
                    token.move_to(XY(
                        row.outlines.x + offset,
                        row.outlines.y + row.ascent - token.ascent));

                    width = max (width, row.outlines.x + row.outlines.w);
                }
            }

            resize(XY(width, height));
        }

        int length () const
        {
            int length = 0;
            for (const auto & token : *this) length += token.glyphs.size();
            return length;
        }
    };

    ///////////////////////////////////////////////////////////////////////
            
    struct column : widgetarium<line>
    {
        void fill (array<line::data> datae)
        {
            int n = 0; int width = 0; int height = 0;

            reserve(datae.size());

            for (auto && data : datae)
            {
                auto refill = [&]()
                {
                    if (size() <= n) return true;

                    if ((*this)(n).data_copy == data) return false;

                    if (size() == datae.size()) return true;

                    // style could be changed by syntax highlighting
                    const auto & tt1 = (*this)(n).data_copy.tokens;
                    const auto & tt2 = data.tokens;
                    bool same_text = tt1.size() == tt2.size();
                    if (same_text)
                        for (int i=0; i<tt1.size(); i++)
                            if (tt1[i].text != tt2[i].text)
                                { same_text = false; break; }

                    if (same_text) return true;

                    if (size() < datae.size())
                    { // lines were inserted
                        emplace_back().fill(std::move(data));
                        rotate(n, size()-1, size());
                        return false;
                    }
                    else
                    { // lines were removed
                        rotate(n, size()-datae.size()+n, size());
                        truncate(datae.size());
                        return (*this)(n).data_copy != data;
                    }
                };

                if (refill())
                    (*this)(n).fill(std::move(data));

                line & line = (*this)(n);
                line.move_to(XY(0, height));
                width = max(width, line.coord.now.size.x);
                height += line.coord.now.size.y;
                n++;
            }

            truncate(n);

            resize(XY(width, height));
        }        

        range point (XY p)
        {
            range point;
            point.from.line = -1;
            point.upto.line = -1;

            for (auto & line : *this)
            {
                XY lp = p - line.coord.now.origin;
                if (lp.y < 0) break;
                point.from.line++;
                point.from.offset = 0;
                point.upto = point.from;
                if (lp.y >= line.coord.now.size.y) {
                    point.from.offset += line.length();
                    point.upto = point.from;
                    continue;
                }

                for (auto & token : line)
                {
                    XY tp = lp - token.coord.now.origin;
                    if (tp.y < 0) break;
                    if (tp.y >= token.coord.now.size.y ||
                        tp.x >= token.coord.now.size.x) {
                        point.from.offset += token.size();
                        point.upto = point.from;
                        continue;
                    }

                    for (auto & glyph : token)
                    {
                        XY gp = tp - glyph.coord.now.origin;
                        if (gp.x < 0) return point;
                        point.from.offset = point.upto.offset;
                        point.upto.offset++;
                    }
                }
            }

            return point;
        }

        array<XYWH> bars (range range)
        {
            array<XYWH> bars;

            auto [from, upto] = range;
            
            if (from > upto) std::swap (from, upto);

            for (; from.line <= upto.line; from.line++, from.offset = 0)
            {
                if (from.line >= size()) break;
                int from_offset = from.offset;
                int upto_offset = from.line == upto.line ?
                    upto.offset : (*this)(from.line).length();
                if (from_offset >= upto_offset) continue;

                int offset = 0;

                for (auto & token : (*this)(from.line))
                {
                    if (upto_offset <= offset) break;
                    if (from_offset <= offset + token.size() - 1)
                    {
                        int from_glyph = max(from_offset - offset, 0);
                        int upto_glyph = min(upto_offset - offset, token.size());

                        auto & g1 = token(from_glyph);
                        auto & g2 = token(upto_glyph-1);
                        bars += XYXY (
                            g1.coord.now.x,
                            g1.coord.now.y,
                            g2.coord.now.x + g2.coord.now.w,
                            g2.coord.now.y + g2.coord.now.h
                        )
                        + token.coord.now.origin
                        + (*this)(from.line).coord.now.origin;
                    }

                    offset += token.size();
                }
            }

            return bars;
        }

        XYWH bar (place place, bool virtual_space)
        {
            if (place.line >= size()) return XYWH{};
            if (place.offset < 0)
                place.offset = 0;

            int offset = 0;

            for (auto & token : (*this)(place.line))
            {
                if (place.offset - offset < token.size())
                    return token(place.offset - offset).coord.now
                    + token.coord.now.origin
                    + (*this)(place.line).coord.now.origin;

                offset += token.size();
            }

            style_index style;

            for (int line = place.line; line >= 0; line--)
                if ((*this)(line).size() > 0) { style =
                    (*this)(line).back().style; break; }

            sys::glyph space (" ", style);

            XYWH r = (*this)(place.line).coord.now;
            r.x = r.x + r.w + (place.offset - offset) * space.width;
            r.w = space.width;
            return r;
        }
    };
} 