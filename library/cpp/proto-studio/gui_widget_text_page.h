#pragma once
#include "../sys.h"
#include "doc.h"
#include "gui_widget.h"
#include "gui_widgetarium.h"
#include "gui_widget_text_aux.h"
namespace gui::text
{
    struct token final : widgetarium<glyph>, glyph_metrics
    {
        struct data { str text; glyph_style_index style; };

        str text; glyph_style_index style; void fill (
        str text, glyph_style_index style)
        {
            if (this->text == text && this->style == style) return;
                this->text =  text;   this->style =  style;

            width = ascent = descent = advance = 0;

            glyphs = unicode::glyphs(text);
            for (int i=0; i<glyphs.size(); i++)
            {
                glyph & glyph = i < size() ? (*this)(i) : emplace_back();
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
            
    struct row : glyph_metrics
    {
        format format;
        array<int> offsets;
        bool the_last_row = true;

        bool add (token & token)
        {
            int Width =
                format.width -
                format.margin_left.x -
                format.margin_right.x; 

            if (format.word_wrap
            &&  Width < width + advance + token.width
            &&  offsets.size() > 0) // at least one should be accepted
            {
                the_last_row = false;
                return false;
            }

            ascent  = max (ascent,  token.ascent);
            descent = max (descent, token.descent);
            offsets += width + advance;
            width = offsets.back() + token.width;
            advance = token.advance;
            outlines.size.y = ascent + descent;
            outlines.size.x = token.text == " " || token.text == "\n" ?
            outlines.size.x : width;
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
        array<row> rows;

        void fill (array<token::data>::range lexemes, format format)
        {
            rows.clear();

            if (format.width  <= 0) { resize(XY()); return; }
            if (format.height <= 0) { resize(XY()); return; }

            if (format.width == max<int>() && format.alignment.x != left)
                throw std::out_of_range("text::line: format impossible");

            int total = 0; int height = 0;

            for(auto [lexeme, n] : lexemes)
            {
                token & token = n < size() ? (*this)(n) : emplace_back();
                token.fill(lexeme.text, lexeme.style);

                if (rows.size() == 0 || !rows.back().add(token))
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
                    rows.back().add(token); // at least one should be accepted
                }

                total++;
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
    };

    ///////////////////////////////////////////////////////////////////////
            
    struct section final : widgetarium<line>
    {
        struct data { format format; array<token::data> tokens; };

        void fill (data data)
        {
            auto & format = data.format;
            auto & tokens = data.tokens;

            int lines = 0; int width = 0; int height = 0;

            for (auto i = tokens.begin(); i != tokens.end(); )
            {
                auto j = std::find_if(i, tokens.end(),
                [](auto token){ return token.text == "\n"; });
                if (j != tokens.end()) j++;

                line & line = lines < size() ? (*this)(lines) : emplace_back();
                line.fill(tokens
                    .from((int)(i - tokens.begin()))
                    .upto((int)(j - tokens.begin()))
                    , format);
                line.move_to(XY(0, height));
                lines++;

                int w = line.coord.now.size.x;  width = max(width, w);
                int h = line.coord.now.size.y;  height += h; 

                format.height         = max (0, format.height         - h);
                format.margin_left.y  = max (0, format.margin_left.y  - h);
                format.margin_right.y = max (0, format.margin_right.y - h);

                if (format.height == 0) break;

                i = j;
            }

            truncate(lines);

            resize(XY(width, height));
        }        
    };

    ///////////////////////////////////////////////////////////////////////
            
    struct page final : widgetarium<section>
    {
        void fill (const array<section::data> & datae)
        {
            int n = 0; int width = 0; int height = 0;

            for (const auto & data : datae)
            {
                section & section = n < size() ? (*this)(n) : emplace_back();
                section.fill(data);
                section.move_to(XY(0, height));
                n++;

                width = max(width, section.coord.now.size.x);
                height += section.coord.now.size.y;
            }

            truncate(n);

            resize(XY(width, height));
        }        
    };
} 