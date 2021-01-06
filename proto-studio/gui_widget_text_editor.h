#pragma once
#include "doc_text_model_a.h"
#include "gui_widget_text_aux.h"
#include "gui_widget_text_page.h"
namespace gui::text
{
    struct editor:
    widget<editor>, doc::view::model
    {
        page page;
        binary_property<bool> insert_mode = true;
        binary_property<bool> virtual_space = false;

        doc::text::model _model;
        doc::text::model* model = &_model;
        std::map<str, style_index> styles;

        editor ()
        {
            page.view.model = this;
            page.alignment = XY{pix::left, pix::top};
        }

        void reset ()
        {
            update(); // speed up rectifier
            page.view.column.clear(); // reset cache
            //model->proceed(model, model->tokens);
            page.view.refresh();
            page.refresh();
            refresh();
            notify();
        }

        //void set (str text, str format)
        //{
        //    update(); // speed up
        //    page.view.column.clear(); // reset cache
        //    //model = doc::text::model{text};
        //    page.view.refresh();
        //    page.refresh();
        //    refresh();
        //    notify();
        //}

        void set (style s, format f) override
        {
            lines = {doc::view::line{f}};
        
            //for (const auto & t : model->tokens)
            //{
            //    if (t.text == "\n")
            //    {
            //        if (virtual_space.now)
            //        lines.back().tokens += doc::view::token{str(' ', 128), s}; // huge slowdown
            //        lines.back().tokens += doc::view::token{"\n", s};
            //        lines += {line::data{f}};
            //    }
            //    else
            //    {
            //        glyph_style_index style = s;
            //        if (auto it = styles.find(t.kind);
            //            it != styles.end())
            //            style = it->second;
            //
            //        lines.back().tokens += token::data{t.text, style};
            //    }
            //}
        }

        void refresh ()
        {
            array<range> selections;
            for (auto s : model->selections)
                selections += range{
                    place{s.from.line, s.from.offset},
                    place{s.upto.line, s.upto.offset}};
                
            page.view.selections = selections;

            if (page.view.carets.size() == 0) return;
            
            XYXY r = page.view.carets.back().coord.now;

            int d = gui::metrics::text::height;
            int w = coord.now.size.x, dx = 0;
            int h = coord.now.size.y, dy = 0;

            if (r.xl-d < 0) dx = r.xl-d; else if (r.xh+d > w) dx = r.xh+d-w;
            if (r.yl-d < 0) dy = r.yl-d; else if (r.yh+d > h) dy = r.yh+d-h;

            if (dx != 0) page.scroll.x.top = page.scroll.x.top.now + dx;
            if (dy != 0) page.scroll.y.top = page.scroll.y.top.now + dy;
        }

        void on_change (void* what) override
        {
            if (what == &coord && coord.now.size != coord.was.size)
            {
                page.coord = coord.now.local();
            }
            if (what == &virtual_space)
            {
                page.view.virtual_space = virtual_space.now;
            }
            if (what == &insert_mode)
            {
                page.view.insert_mode = insert_mode.now;
            }
        }

        enum WHERE { THERE = 0,
             GLYPH, LINE, LINE_BEGIN, LINE_END, PAGE_TOP,
             TOKEN, PAGE, TEXT_BEGIN, TEXT_END, PAGE_BOTTOM,
        };

        void go (int where, bool selective = false)
        {
            int n = model->selections.size();
            if (n >= 2 && !selective) {
                model->selections[0] = 
                model->selections[n-1];
                model->selections.resize(1);
            }

            for (auto & caret : model->selections)
                go(caret, where, selective);

            refresh();
        }
        void go (range & caret, int where, bool selective)
        {
            auto & [from, upto] = caret;
            auto & line = model->lines[upto.line];

            switch(where){
            case THERE: from = caret.upto; break;

            case-GLYPH: upto.offset--;
                if (upto.offset < 0 && virtual_space.now) upto.offset++; else
                if (upto.offset < 0 && upto.line > 0) {
                    upto.offset = model->lines[upto.line].size();
                    upto.line--;
                }
                break;
            case+GLYPH: upto.offset++;
                if (upto.offset > line.size() && virtual_space.now) {;} else
                if (upto.offset > line.size() && upto.line < model->lines.size()-1) {
                    upto.offset = 0;
                    upto.line++;
                }
                break;

            case LINE_END  : upto.offset = line.size(); break;
            case LINE_BEGIN:
                {
                    auto i = line.find_if([](auto s){ return s != " "; });
                    if (i == line.end()) upto.offset = 0; else {
                        int n = (int)(i - line.begin());
                        upto.offset = upto.offset != n ? n : 0;
                    }
                }
                break;

            case-LINE: upto.line--;
                if (upto.line < 0) upto.line++; else
                if (upto.offset > model->lines[upto.line].size() && !virtual_space.now)
                    upto.offset = model->lines[upto.line].size();
                break;
            case+LINE: upto.line++;
                if (upto.line  >= model->lines.size()) upto.line--; else
                if (upto.offset > model->lines[upto.line].size() && !virtual_space.now)
                    upto.offset = model->lines[upto.line].size();
                break;

            //case-TOKEN: break;
            //case+TOKEN: break;

            case-PAGE: upto.line -= page.view.coord.now.h /
                sys::metrics(page.view.font.now).height;
                if (upto.line < 0)
                    upto.line = 0;
                break;
            case+PAGE: upto.line += page.view.coord.now.h /
                sys::metrics(page.view.font.now).height;
                if (upto.line > model->lines.size() - 1)
                    upto.line = model->lines.size() - 1;
                break;

            //case PAGE_TOP   : break;
            //case PAGE_BOTTOM: break;

            case TEXT_BEGIN: upto.line = 0; upto.offset = 0; break;
            case TEXT_END  : upto.line = model->lines.size()-1;
                upto.offset = model->lines[upto.line].size();
                break;
            }

            if (!selective) from = upto;
        }

        void go (place place)
        {
            model->selections.resize(1);
            model->selections.back().from = 
            model->selections.back().upto = place;

            page.scroll.y.top = 
            sys::metrics(page.view.font.now).height * place.line
            - page.view.coord.now.h / 2;

            refresh();
        }

        void show (int where)
        {
            switch(where){
            case-LINE:
                page.scroll.y.top = page.scroll.y.top.now
                - sys::metrics(page.view.font.now).height;
                break;
            case+LINE:
                page.scroll.y.top = page.scroll.y.top.now
                + sys::metrics(page.view.font.now).height;
                break;
            case-PAGE:
                page.scroll.y.top = page.scroll.y.top.now - page.view.coord.now.h
                / sys::metrics(page.view.font.now).height
                * sys::metrics(page.view.font.now).height;
                break;
            case+PAGE:
                page.scroll.y.top = page.scroll.y.top.now + page.view.coord.now.h
                / sys::metrics(page.view.font.now).height
                * sys::metrics(page.view.font.now).height;
                break;
            }
        }

        str selected () const
        {
            array<str> ss;

            for (auto [from, upto] : page.view.selections.now)
            {
                if (from > upto) std::swap (from, upto);

                for (place p = from; p <= upto; p.offset = 0, p.line++) {
                    ss += doc::text::string(p.line == upto.line ?
                        model->lines[p.line].from(p.offset).upto(upto.offset) :
                        model->lines[p.line].from(p.offset));
                }
            }

            return str(ss);
        }

        void update_view ()
        {
            page.view.refresh();
            page.refresh();
            refresh();
            notify();
        }
        void undo        () { if (model->undo     ()) update_view(); }
        void redo        () { if (model->redo     ()) update_view(); }
        void erase       () { if (model->erase    ()) update_view(); }
        void backspace   () { if (model->backspace()) update_view(); }
        void insert (str s) { if (model->insert  (s)) update_view(); }

        void on_key_pressed (str key, bool down) override
        {
            if (!down) return;
            if (touch) return; // mouse
            if (key == "space" ) return;
            if (key.size() <= 1) return; // "", "0", "A", ...
            if (key.size() <= 7 && key.starts_with("shift+")) return; // shift+0, shift+A, ...

            if (key == "ctrl+C") key = "ctrl+insert"; else // copy
            if (key == "ctrl+V") key = "shift+insert"; else // paste
            if (key == "ctrl+X") key = "shift+delete"; else // cut
            if (key == "ctrl+Z") key = "alt+backspace"; else // undo

            if (key == "ctrl+Y"      ) key = "ctrl+backspace"; else // redo
            if (key == "ctrl+shift+Z") key = "ctrl+backspace"; else // redo
            {}

            if (key == "alt+shift+left" ) key = "shift+left";
            if (key == "alt+shift+right") key = "shift+right";

            if (key == "alt+shift+up")
            {
                int n = model->selections.size();
                if (n >= 2 &&
                    model->selections[n-2].upto.line == 
                    model->selections[n-1].upto.line - 1)
                    model->selections.truncate();
                else
                if (n >= 1 &&
                    model->selections[n-1].upto.line < model->lines.size())
                    model->selections += range{
                        {model->selections[n-1].from.line-1, model->selections[n-1].from.offset},
                        {model->selections[n-1].upto.line-1, model->selections[n-1].upto.offset}};
                else return;
                refresh();
            }
            else
            if (key == "alt+shift+down")
            {
                int n = model->selections.size();
                if (n >= 2 &&
                    model->selections[n-2].upto.line == 
                    model->selections[n-1].upto.line + 1)
                    model->selections.truncate();
                else
                if (n >= 1 &&
                    model->selections[n-1].upto.line > 0)
                    model->selections += range{
                        {model->selections[n-1].from.line+1, model->selections[n-1].from.offset},
                        {model->selections[n-1].upto.line+1, model->selections[n-1].upto.offset}};
                else return;
                refresh();
            }
            else

            if (key == "left" ) go(-GLYPH); else
            if (key == "right") go(+GLYPH); else
            if (key == "up"   ) go(-LINE); else
            if (key == "down" ) go(+LINE); else

            if (key == "ctrl+left" ) go(-TOKEN); else
            if (key == "ctrl+right") go(+TOKEN); else
            if (key == "ctrl+up"   ) show(-LINE); else
            if (key == "ctrl+down" ) show(+LINE); else

            if (key == "shift+left" ) go(-GLYPH, true); else
            if (key == "shift+right") go(+GLYPH, true); else
            if (key == "shift+up"   ) go(-LINE,  true); else
            if (key == "shift+down" ) go(+LINE,  true); else

            if (key == "ctrl+shift+left" ) go(-TOKEN, true); else
            if (key == "ctrl+shift+right") go(+TOKEN, true); else
            if (key == "ctrl+shift+up"   ) go(-LINE,  true); else
            if (key == "ctrl+shift+down" ) go(+LINE,  true); else

            if (key == "home"     ) go(LINE_BEGIN); else
            if (key == "end"      ) go(LINE_END  ); else
            if (key == "page up"  ) { show(-PAGE); go(-PAGE); } else
            if (key == "page down") { show(+PAGE); go(+PAGE); } else

            if (key == "ctrl+home"     ) go(TEXT_BEGIN ); else
            if (key == "ctrl+end"      ) go(TEXT_END   ); else
            if (key == "ctrl+page up"  ) go(PAGE_TOP   ); else
            if (key == "ctrl+page down") go(PAGE_BOTTOM); else

            if (key == "shift+home"     ) go(LINE_BEGIN, true); else
            if (key == "shift+end"      ) go(LINE_END,   true); else
            if (key == "shift+page up"  ) { show(-PAGE); go(-PAGE, true); } else
            if (key == "shift+page down") { show(+PAGE); go(+PAGE, true); } else

            if (key == "ctrl+shift+home"     ) go(TEXT_BEGIN , true); else
            if (key == "ctrl+shift+end"      ) go(TEXT_END   , true); else
            if (key == "ctrl+shift+page up"  ) go(PAGE_TOP   , true); else
            if (key == "ctrl+shift+page down") go(PAGE_BOTTOM, true); else

            if (key == "insert"           ) { insert_mode = !insert_mode.now; } else
            if (key == "shift+insert"     ) { insert(sys::clipboard::get::string()); } else
            if (key == "ctrl+insert"      ) { sys::clipboard::set(selected()); } else
            if (key == "ctrl+shift+insert") {} else // VS: clipboard contex menu

            if (key == "delete"           ) { erase(); } else
            if (key == "shift+delete"     ) { sys::clipboard::set(selected()); erase(); } else
            if (key == "ctrl+delete"      ) {} else
            if (key == "ctrl+shift+delete") {} else

            if (key == "enter"            ) { insert("\n"); } else
            if (key == "shift+enter"      ) { insert("\n"); } else
            if (key == "ctrl+enter"       ) { go(LINE_BEGIN); insert("\n"); } else
            if (key == "ctrl+shift+enter" ) { go(LINE_END  ); insert("\n"); } else

            if (key == "backspace"        ) { backspace(); } else
            if (key == "shift+backspace"  ) { backspace(); } else
            if (key == "alt+backspace"    ) { undo(); } else
            if (key == "ctrl+backspace"   ) { redo(); } else // != VS

            if (key == "tab"              ) { insert("\t"); } else
            if (key == "shift+tab"        ) { insert("shift+\t"); } else
            if (key == "escape"           ) { go(THERE); } else

            {} // insert(" [" + key + "] "); // (down ? " (down) " : " (up) ");
        }
        void on_keyboard_input (str symbol) override
        {
            if (!touch) insert(symbol);
        }
        void on_focus (bool on) override
        {
            page.view.on_focus(on);
        }

        bool touch = false;  XY touch_point; time touch_time;

        bool mouse_sensible (XY p) override { return true; }

        void on_mouse_press (XY p, char button, bool down) override
        {
            if (button != 'L') return;
            bool drag_and_drop = false;
            if (drag_and_drop)
            {
            }
            if (down && !touch)
            {
                if (touch_point == p && time::now - touch_time < 1000ms)
                {
                    go(-TOKEN); go(+TOKEN, true);
                }
                else
                {
                }
            }
            touch = down;
        }
        void on_mouse_hover (XY p) override
        {
            bool drag_and_drop = false;

            bool inside_selection = false;

            mouse_image = drag_and_drop ?
            inside_selection ? "noway" : "arrow" :
            inside_selection ? "arrow" : "editor";

            if (drag_and_drop)
            {
            }
            else
            {
                if (touch)
                {
                    int n = page.view.selections.now.size();
                    model->selections.resize(n);
                    for (int i=0; i<n; i++) {
                        auto r = page.view.selections.now[i];
                        model->selections[i] = range{
                            place{r.from.line, r.from.offset},
                            place{r.upto.line, r.upto.offset}};

                        // hack to prevent focus hiding:
                        auto & [from, upto] = model->selections[i];
                        from = clamp(from, model->front(), model->back());
                        upto = clamp(upto, model->front(), model->back());
                        page.view.selections.now[i] = range{
                            place{from.line, from.offset},
                            place{upto.line, upto.offset}};
                        refresh();
                    }
                }
            }
        }

        void on_notify (void* what) override
        {
            if (what == &page.scroll.x) notify(what);
            if (what == &page.scroll.y) notify(what);
        }
    };
} 