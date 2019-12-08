#pragma once
#include "gui_widget_text_view.h"
namespace gui::text
{
    struct editor final : widget<editor>
    {
        property<int> indent = 0;

        binary_property<bool> insert_mode = true;

        struct caret final : widget<caret>
        {
            canvas canvas;
            property<time> timer;
            binary_property<bool> insert_mode;
            void on_change (void* what) override
            {
                if (what == &coord && coord.now.size != coord.was.size
                ||  what == &insert_mode) {
                    XYWH r = coord.now.local();
                    if (insert_mode.now) r.w = max (1, r.h/16);
                    canvas.coord = r;
                    if (timer.now == time())
                        timer.go (time::infinity, time::infinity);
                }
                if (what == &timer) {
                    int ms = time::now.ms % 1024; if (ms <= 512)
                    canvas.alpha = 255*ms/512; else
                    canvas.alpha = 255*(1024-ms-1)/512;
                }
            }
        };

        widgetarium<canvas> highlight;
        widgetarium<canvas> selection;
        view view; // after selection, before carets
        widgetarium<caret> carets;

        void on_change (void* what) override
        {
            if (what == &coord && coord.now.size != coord.was.size)
            {
                auto r = coord.now.local();
                view.coord = r;
                highlight.coord = r;
                selection.coord = r;
                carets.coord = r;

                if (carets.size() == 0)
                    carets.emplace_back();

                auto co = schemas[""].touched.back_color; co.a = 192;
                carets(0).canvas.color = co;
                carets(0).coord = XYWH(0,0,15,30);
                carets(0).insert_mode = false;
            }
        }

        str s;

        void on_focus (bool on) override {}
        void on_key_pressed (str key, bool down) override
        {
            if (key.size() <= 1) return; // "", "0", "A", ...
            if (key == "space") return;
            if (key == "enter") s += "\n"; else
            s += " " + key + (down ? " (down) " : " (up) ");
            view.text = s;
        }
        void on_keyboard_input (str symbol) override
        {
            s += symbol; view.text = s;
        }


        // zero glyph width => zero caret width


        struct line final : widget<line>
        {

            //void set_caret(int position) { carets.clear(); add_caret(position); }
            //void add_caret(int position) { int n = size();
            //     auto i = std::find_if (carets.begin(), carets.end(),
            //         [position](auto & caret){ return caret.position.now <= position; })
            //         - carets.begin(); // linear insertion sort by decreasing positions...
            //     if (i < n && carets(i).position.now == position) return;
            //     carets.emplace_back().position = position;
            //     carets.back().coord = glyphs(position).coord.now;
            //     carets.back().insert_mode = insert_mode.now;
            //     carets.rotate (i, n, n+1); 
            //}
            //
            //void insert (sys::glyph g) {
            //     for (auto & caret : carets) {
            //        if (insert_mode.now)
            //            insert (caret.position.now, g); else
            //            replace(caret.position.now, g);
            //        caret.position = caret.position.now + 1;
            //        caret.coord = glyphs(caret.position.now).coord.now;
            //     }
            //}
            //void insert (sys::token token) {
            //     for (auto & caret : carets) {
            //        insert(caret.position.now, token);
            //        caret.position = caret.position.now + token.glyphs.size();
            //     }
            //}

            //  void on_change (void* what) override {
            //      if (what == &coord) {
            //          canvases.resize(coord.now.size);
            //          for (auto & caret : carets)
            //              caret.coord = glyphs(caret.position.now).coord.now;
            //      }
            //      if (what == &insert_mode) {
            //          for (auto & caret : carets)
            //              caret.insert_mode = insert_mode.now;
            //      }
            //  }
        };

        struct model final : widget<model>
        {
            typedef doc::Document Document;

            struct context
            {
                Document* document; int top = 0;
            };

            //Document internal_document;
            //context  internal_context {&internal_document};
            //context* context = &internal_context;
            context contextt;

            void insert (str text) { insert (std::move(text), contextt.document->text.size()); }
            void insert (str text, int pos)
            {
                contextt.document->insert(text, pos);
                notify();
            }
        };
        //struct view final : widget<view>
        //{
        //    page page;
        //    //scroll scroll;
        //    const model* model;
        //
        //    void refresh ()
        //    {
        //        for (auto token : model->contextt.document->syntax)
        //        {
        //
        //        }
        //    }
        //};
        //struct controller
        //{
        //    model* model;
        //};
        //
        //model model; view view; controller controller;
        //
        //editor () { view.model = &model; controller.model = &model; }

        //void on_notify (gui::base::widget* w) override { if (w == &model) view.refresh(); }
    };
} 