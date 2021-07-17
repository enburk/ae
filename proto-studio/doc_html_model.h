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
            if (entity.kind == "text")
            {
                if (lines.size() == 0 ||
                    lines.back().format != formats.back())
                    lines += line{formats.back()};

                for (auto token : entity.head)
                    lines.back().tokens +=
                        doc::view::token{token.text,
                            style_index(styles.back()),
                            links.back()};
            }
            else
            if (entity.name == "br")
            {
                if (lines.size() == 0)
                lines += line{formats.back()};
                lines.back().tokens += token{"\n", style_index(styles.back())};
                lines += line{formats.back()};
            }
            else
            if (entity.name == "h4") {
                styles += styles.back();
                styles.back().font.bold = true;
            }
            else
            if (entity.name == "b") {
                styles += styles.back();
                styles.back().font.bold = true;
            }
            else
            if (entity.name == "i") {
                styles += styles.back();
                styles.back().font.italic = true;
            }
            else
            if (entity.name == "small") {
                int size = gui::metrics::text::height;
                styles += styles.back();
                styles.back().font.size = size*85/100;
            }
            else
            if (entity.name == "big") {
                int size = gui::metrics::text::height;
                styles += styles.back();
                styles.back().font.size = size*115/100;
            }
            else
            if (entity.name == "sub") {
                int size = gui::metrics::text::height;
                styles += styles.back();
                styles.back().font.size = size*75/100;
                styles.back().shift.y = size*50/100;
            }
            else
            if (entity.name == "sup") {
                int size = gui::metrics::text::height;
                styles += styles.back();
                styles.back().font.size = size*75/100;
                styles.back().shift.y = -size*50/100;
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
                auto style = styles.back();

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
                auto style = styles.back();
                int x = 3 * sys::metrics(style.font).height;
                x += formats.back().margin_left.x;
                formats += formats.back();
                formats.back().margin_left = pix::XY(x, max<int>());
            }
            else
            if (entity.name == "div")
            {
                auto style = styles.back();

                for (auto [attr, value] : entity.attr)
                {
                    if (attr == "style") // <div style="margin-left: 9em">
                    {
                        str arg, val;
                        value.strip("\"");
                        value.split_by(":", arg, val);
                        arg.strip();
                        val.strip();

                        if (arg == "margin-left")
                        {
                            if (val.ends_with("em")) {
                                val.truncate();
                                val.truncate();
                                int x = std::atoi(val.c_str());
                                x = x * sys::metrics(style.font).height;
                                // x += formats.back().margin_left.x;
                                formats += formats.back();
                                formats.back().margin_left = pix::XY(x, max<int>());
                            }
                        }

                        if (arg == "line-height")
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
            }
            else
            if (entity.name == "span")
            {
                auto style = styles.back();

                for (auto [attr, value] : entity.attr)
                {
                    if (attr == "style")
                    {
                        value.strip("\"");
                        for (str s : value.split_by(";"))
                        {
                            if (s == "font-variant:small-caps") {
                                styles += styles.back();
                                styles.back().font.face = 
                                "small-caps";
                            }
                            if (s == "font-family:monospace") {
                                styles += styles.back();
                                styles.back().font.face = 
                                "monospace";
                            }
                            if (s == "font-style:normal") {
                                styles += styles.back();
                                styles.back().font.bold = false;
                                styles.back().font.italic = false;
                            }
                        }
                    }
                }
            }

            for (auto e : entity.body)
                proceed(e, styles, formats, links);

            if (entity.name == "h4") {
                if (lines.size() > 0 &&
                    lines.back().tokens.size() > 0) {
                    lines.back().tokens += token{"\n", style_index(styles.back())};
                    lines += line{formats.back()};
                }
            }

        }

    };
} 