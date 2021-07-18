#pragma once
#include "doc.h"
#include "doc_html_utils.h"
namespace doc::html
{
    using namespace doc::view;
    struct model : doc::view::model
    {
        str source;
    //  array<line> lines;
        array<entity> entities;

        str text () const { return untagged(source); }
        str html () const { return source; }

        void text (str text) { html(encoded(text)); }
        void html (str text)
        {
            source = std::move(text);
            entities = html::entities(source);
            if (false) { // debug
                auto tokens = print(entities);
                entities.clear();
                entities += entity{"", "text"};
                entities.back().head = tokens;
            }
        }

        void set (style s, format f) override
        {
            lines.clear();

            array<style>  styles  {s}; // stack
            array<format> formats {f}; // stack
            array<str>    links  {""}; // stack

            for (auto entity : entities)
                proceed(entity, styles, formats, links);
        }

        void proceed
        (
            entity const& entity,
            array<style>  styles,
            array<format> formats,
            array<str>    links
        )
        {
            auto link = links.back();
            auto style = styles.back();
            auto format = formats.back();

            int height = sys::metrics(style.font).height;

            std::map<str,str> attr_style;
            for (auto [attr, value] : entity.attr)
            {
                if (attr == "style")
                {
                    value.strip("\"");
                    for (str s : value.split_by(";"))
                    {
                        str key, value;
                        if (s.split_by(":", key, value))
                            attr_style[key] = value;
                    }
                }
            }

            auto heights = [height](str val)
            {
                double h = 0;
                if (val.ends_with("em")) h = height; else
                if (val.ends_with("ex")) h = height * 0.39; else
                return h;
                val.truncate();
                val.truncate();
                return h * std::atof(val.c_str());
            };

            if (entity.kind == "text")
            {
                if (lines.size() == 0 ||
                    lines.back().format != format)
                    lines += line{format};

                for (auto token : entity.head)
                    lines.back().tokens +=
                        doc::view::token{token.text,
                            style_index(style),
                            link};
            }
            else
            if (entity.name == "br")
            {
                if (lines.size() == 0)
                lines += line{format};
                lines.back().tokens += token{"\n", style_index(style)};
                lines += line{format};
            }
            else
            if (entity.name == "h4") {
                style.font.bold = true;
                styles += style;
            }
            else
            if (entity.name == "b") {
                style.font.bold = true;
                styles += style;
            }
            else
            if (entity.name == "i") {
                style.font.italic = true;
                styles += style;
            }
            else
            if (entity.name == "small") {
                style.font.size = height * 85/100;
                styles += style;
            }
            else
            if (entity.name == "big") {
                style.font.size = height * 125/100;
                styles += style;
            }
            else
            if (entity.name == "sub") {
                style.font.size = height * 77/100;
                style.shift.y   = height * 30/100;
                for (auto [key, val] : attr_style)
                    if (key == "margin-left")
                        style.shift.x = heights(val);
                styles += style;
            }
            else
            if (entity.name == "sup") {
                style.font.size = height * 77/100;
                style.shift.y  = -height * 30/100;
                for (auto [key, val] : attr_style)
                    if (key == "margin-left")
                        style.shift.x = heights(val);
                styles += style;
            }
            else
            if (entity.name == "a")
            {
                for (auto [attr, value] : entity.attr)
                    if (attr == "href") {
                        value.strip("\"");
                        links += value; }
            }
            else
            if (entity.name == "font")
            {
                for (auto [attr, value] : entity.attr)
                {
                    if (attr == "face")
                    {
                        value.strip("\"");
                        style.font.face = value;
                    }
                    if (attr == "size")
                    {
                        value.strip("\"");
                        if (value.ends_with("%")) {
                            value.truncate();
                            int x = std::atoi(value.c_str());
                            int size = gui::metrics::text::height;
                            style.font.size = size * x/100;
                        }
                    }
                    if (attr == "color" && // <font color=#008000>
                        value.starts_with("#") &&
                        value.size() == 1+6)
                    {
                        str r = value.from(1).upto(3);
                        str g = value.from(3).upto(5);
                        str b = value.from(5).upto(7);
                        style.color.r = (uint8_t) std::strtoul(r.c_str(), nullptr, 16);
                        style.color.g = (uint8_t) std::strtoul(g.c_str(), nullptr, 16);
                        style.color.b = (uint8_t) std::strtoul(b.c_str(), nullptr, 16);
                        style.color.a = 255;
                    }
                }
                styles += style;
            }
            else
            if (entity.name == "blockquote")
            {
                int x = 3 * height + format.margin_left.x;
                format.margin_left = pix::XY(x, max<int>());
                formats += format;
            }
            else
            if (entity.name == "div")
            {
                for (auto [key, val] : attr_style)
                {
                    if (key == "margin-left") {
                        format.margin_left = pix::XY(heights(val), max<int>());
                        formats += format;
                    }

                    if (key == "line-height")
                    {
                        if (val.ends_with("%")) {
                            val.truncate();
                            int x = std::atoi(val.c_str());
                            int size = gui::metrics::text::height;
                            style.font.size = size * x/100;
                            styles += style;
                        }
                    }
                }
            }
            else
            if (entity.name == "span")
            {
                for (auto [key, val] : attr_style)
                {
                    if (key == "font-variant") {
                        style.font.face = val;
                        styles += style;
                    }
                    if (key == "font-family") {
                        style.font.face = val;
                        styles += style;
                    }
                    if (key == "font-style" and
                        val == "normal") {
                        style.font.bold = false;
                        style.font.italic = false;
                        styles += style;
                    }
                }
            }

            for (auto e : entity.body)
                proceed(e, styles, formats, links);

            if (entity.name == "h4") {
                if (lines.size() > 0 &&
                    lines.back().tokens.size() > 0) {
                    lines.back().tokens += token{"\n", style_index(style)};
                    lines += line{format};
                }
            }

        }

    };
} 