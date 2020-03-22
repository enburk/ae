#pragma once
#include "doc.h"
#include "gui_widget.h"
namespace gui::text
{
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