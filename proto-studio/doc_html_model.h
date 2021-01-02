#pragma once
#include "doc_html_utils.h"
namespace doc::html
{
    struct model
    {
        str source;
        array<line> lines;
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

        void set (style s, format f)
        {
            lines.clear();

            array<style>  styles  {s}; // stack
            array<format> formats {f}; // stack

            for (auto entity : entities)
                proceed(entity, styles, formats);
        }

        void proceed
        (
            entity const& entity,
            array<style>  styles,
            array<format> formats
        )
        {
            if (entity.kind == "text")
            {
                if (lines.size() == 0 ||
                    lines.back().format != formats.back())
                    lines += line{formats.back()};

                for (auto token : entity.head)
                    lines.back().tokens +=
                        doc::token{token.text,
                            style_index(styles.back())};
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
            if (entity.name == "font")
            {
                for (auto [attr, value] : entity.attr)
                {
                    if (attr == "color" && // <font color=#008000>
                        value.starts_with("#") &&
                        value.size() == 1+6)
                    {
                        styles += styles.back();
                        auto & style = styles.back();
                        str r = value.from(1).upto(3);
                        str g = value.from(3).upto(5);
                        str b = value.from(5).upto(7);
                        style.color.r = (uint8_t) std::strtoul(r.c_str(), nullptr, 16);
                        style.color.g = (uint8_t) std::strtoul(g.c_str(), nullptr, 16);
                        style.color.b = (uint8_t) std::strtoul(b.c_str(), nullptr, 16);
                        style.color.a = 255;
                    }
                }
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
                                formats.back().margin_left = XY(x, max<int>());
                            }
                        }

                        if (arg == "line-height")
                        {
                            if (val.ends_with("%")) {
                                val.truncate();
                                int x = std::atoi(val.c_str());
                                style.font.size = style.font.size * x/100;
                                styles += style;
                            }
                        }
                    }
                }
            }

            for (auto e : entity.body)
                proceed(e, styles, formats);

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