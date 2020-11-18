#pragma once
#include "doc.h"
#include "doc_html_syntax.h"
#include "gui_widget.h"
#include "gui_widget_text_aux.h"
namespace gui::text
{
    struct model : polymorphic
    {
        array<line::data> lines;
        virtual str text () { return ""; }
        virtual str html () { return ""; }
        virtual void set (str text, str format) {}
        virtual void set (style_index, format) {}
    };

    struct html_model : model
    {
        array<doc::entity> entities; str source;

        str text () override { return doc::html::lexica::untagged(source); }
        str html () override { return source; }

        void set (str text, str format) override
        {
            if (format == "text")
            {
                text = doc::html::lexica::encoded(text);
                text.replace_all("\n", "<br>");
            }

            source = text;
            entities =
                doc::html::syntax::combine(
                    doc::html::syntax::parse(
                        doc::html::lexica::parse(
                            doc::text(text))));

            if (false) { // debug
                auto tokens = doc::html::syntax::print(entities);
                entities.clear();
                entities += doc::entity{"", "text"};
                entities.back().head = tokens;
            }
        }

        void set (style_index s, format f) override
        {
            lines.clear();

            array<format> formats {f}; // stack
            array<style_index> styles {s}; // stack

            for (auto entity : entities)
                proceed(entity, styles, formats);
        }

        void proceed
        (
            const doc::entity & entity,
            array<style_index> styles,
            array<format> formats
        )
        {
            if (entity.kind == "text")
            {
                if (lines.size() == 0 ||
                    lines.back().format != formats.back())
                    lines += line::data{formats.back()};

                for (auto token : entity.head)
                    lines.back().tokens +=
                        token::data{token.text,
                            styles.back()};
            }
            else
            if (entity.name == "br")
            {
                lines.back().tokens += token::data{"\n", styles.back()};
                lines += line::data{formats.back()};
            }
            else
            if (entity.name == "h4") {
                auto style = styles.back().style(); style.font.bold = true;
                styles += style_index(style);
            }
            else
            if (entity.name == "b") {
                auto style = styles.back().style(); style.font.bold = true;
                styles += style_index(style);
            }
            else
            if (entity.name == "i") {
                auto style = styles.back().style(); style.font.italic = true;
                styles += style_index(style);
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
                        auto style = styles.back().style();
                        str r = value.from(1).upto(3);
                        str g = value.from(3).upto(5);
                        str b = value.from(5).upto(7);
                        style.color.r = (uint8_t) std::strtoul(r.c_str(), nullptr, 16);
                        style.color.g = (uint8_t) std::strtoul(g.c_str(), nullptr, 16);
                        style.color.b = (uint8_t) std::strtoul(b.c_str(), nullptr, 16);
                        style.color.a = 255;
                        styles += style_index(style);
                    }
                }
            }
            else
            if (entity.name == "div")
            {
                auto style = styles.back().style();

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
                                styles += style_index(style);
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
                    lines.back().tokens += token::data{"\n", styles.back()};
                    lines += line::data{formats.back()};
                }
            }

        }

    };
} 