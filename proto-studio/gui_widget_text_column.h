#pragma once
#include "gui_widget_text_line.h"
namespace gui::text
{
    struct column : widgetarium<line>
    {
        void fill (array<doc::view::line> datae)
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