#pragma once
#include "gui_widgetarium.h"
#include "gui_widget_text_aux.h"
namespace gui::text
{
    struct token final : widgetarium<glyph>, metrics
    {
        array<str> glyphs;
        str text; str info; style_index style; void fill (
        str text, str info, style_index style)
        {
                this->info =  info;
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
    };

    ///////////////////////////////////////////////////////////////////////

    struct row : metrics
    {
        pix::text::format format;
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

            if (align == pix::left ||
               (align == pix::justify_left && the_last_row)) {
                return;
            }

            if (align == pix::center && Width > width) {
                outlines.x += Width/2 - width/2;
                return;
            }

            if (align == pix::right ||
               (align == pix::justify_right && the_last_row)) {
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
        doc::view::line data_copy;
        array<row> rows;

        void fill (doc::view::line data)
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

            int n = -1;
            for(auto lexeme : tokens)
            {
                n++;
                token & token = (*this)(n);
                token.fill(lexeme.text, lexeme.info, lexeme.style);

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
            for (const auto & token : *this) length += token.size();
            return length;
        }
    };
} 