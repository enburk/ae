#pragma once
#include "doc.h"
#include "doc_syntax_html.h"
#include "gui_widget.h"
#include "gui_widget_text_aux.h"
namespace gui::text
{
    struct htmlmodel
    {
        array<line::data> lines;

        htmlmodel (){}
        htmlmodel (const array<doc::entity> & entities, glyph_style_index s, format f)
        {
            array<format> formats {f}; // stack
            array<glyph_style_index> styles {s}; // stack

            for (auto entity : entities)
                proceed(entity, styles, formats);
        }

        void proceed
        (
            const doc::entity & entity,
            array<glyph_style_index> styles,
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
                styles += glyph_style_index(style);
            }
            else
            if (entity.name == "b") {
                auto style = styles.back().style(); style.font.bold = true;
                styles += glyph_style_index(style);
            }
            else
            if (entity.name == "i") {
                auto style = styles.back().style(); style.font.italic = true;
                styles += glyph_style_index(style);
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
                        styles += glyph_style_index(style);
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
                                styles += glyph_style_index(style);
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





    struct model
    {
        doc::document idocument; // internal
        doc::document* document = &idocument;

        XY size;

        struct caret : doc::range {};
        array<caret> carets = {caret{}};
        array<array<caret>> undoes;
        array<array<caret>> redoes;

        void undo () { 
//            if (undoes.size() == 0) return;
//            auto undo = undoes.back(); undoes.pop_back();
//            for (int i=0; i<undoes.size(); i++) document->text.undo();
//            redoes += undo;
        }
        void redo () {
//            if (redoes.size() == 0) return;
//            auto redo = redoes.back(); redoes.pop_back();
//            for (int i=0; i<redoes.size(); i++) document->text.redo();
//            undoes += redo;
        }

        void insert (str text, bool insert_mode)
        {
//            redoes.clear();
//            undoes += carets;
//
//            for (int i=0; i<carets.size(); i++)
//            {
//                doc::text::replace replace;
//                replace.range = carets[i];
//                ///text.parse("\n", replace.text, text);
//                if (text == "") text = replace.text;
//
//                if (!insert_mode &&
//                    replace.range.empty())
//                    replace.range.upto.offset++;
//
//                auto range = document->text.perform(replace);
//
//                for (int j=i+1; j<carets.size(); j++)
//                {
//                    carets[j].from.line += range.upto.line - range.from.line;
//                    carets[j].upto.line += range.upto.line - range.from.line;
//                }
//                carets[i].from = range.upto;
//                carets[i].upto = range.upto;
//            }
        }

        str selection ()
        {
            str s;
//            for (auto & caret : carets) {
//                s += document->text.fragment(caret);
//                s += "\n";
//            }
//            s.truncate();
//            if (s.contains_only("\n")) s = "";
            return s;
        }



        void drop_selection (bool caret_goes_to_the_right)
        {
            // int n = model.carets.size();
            // if (n > 1)
            // {
            //     if (where > 0) std::swap(
            //         model.carets[n-1],
            //         model.carets[0]);
            // 
            //     model.carets.truncate(1);
            // }
            // 
            // carets[0].selection = doc::range{
            // carets[0].position,
            // carets[0].position};
        }



        doc::place position (XY p)
        {
            int line = 0;
            int offset = 0;
            line = aux::max(0, aux::min(line, lines.size()));
            return doc::place{line, offset};
        }

        XYWH coord (doc::place where)
        {
        }

        void go (doc::place where, bool selective = false)
        {
            //model.drop_selection(where > 0);
            //
            //int n = model.carets.size();
            //if (n > 1)
            //{
            //    if (where > 0) std::swap(
            //        model.carets[n-1],
            //        model.carets[0]);
            //
            //    model.carets.truncate(1);
            //}
            //
            //if (selective)
            //{
            //    carets[0].selection.upto = carets[0].position;
            //}
            //else
            //{
            //    carets[0].selection.from = carets[0].position;
            //    carets[0].selection.upto = carets[0].position;
            //}
            //
            //
            //
            //str s = model.selection();
            //model.carets.resize(1);
            //if (where == "line begin" && line_begin()) {}

        }


        struct line
        {
            //array<sys::token> tokens;
            XY metrics;
        };
        array<line> lines;


        void refine (doc::range)
        {
        }


    };


    struct base_model
    {
    };
     

    struct model1 : base_model
    {
    };

    struct model2 : base_model
    {
    };

    //struct model : base_model
    //{
    //    model1 model1;
    //    model2 model2;
    //};
} 