#pragma once
#include "gui_widget_text_aux.h"
#include "gui_widget_text_model.h"
#include "gui_widget_text_view.h"
namespace gui::text
{
    struct editor final : widget<editor>
    {
        //doc::Document _document;
        //doc::Document* document = &_document;
        //
        //struct model
        //{
        //    XY shift;
        //    array<XY> metrics;
        //    array<XY> metrics_word_wrap;
        //    struct caret { doc::place position, selection; };
        //    array <caret> carets = {caret{}};
        //}
        //model;

        model model;

        canvas background;
        widgetarium<canvas> highlight;
        widgetarium<canvas> selection;
        view view; // after selection, before carets
        widgetarium<caret> carets;

        binary_property<bool> insert_mode = true;
        binary_property<bool> virtual_space = false;

        property<XY> offset;

        void on_change (void* what) override
        {
            if (what == &coord && coord.now.size != coord.was.size)
            {
                auto r = coord.now.local();
                background.coord = r;
                highlight.coord = r;
                selection.coord = r;
                carets.coord = r;
                view.coord = r;

                refresh();
            }
            if (what == &insert_mode)
            {
                for (auto & caret : carets)
                    caret.insert_mode =
                    insert_mode.now;
            }
        }

        void refresh ()
        {
            //view.tokens = document->lines[0].tokens;

            //for (int i=0; i<model.carets.size(); i++)
            //{
            //    auto & caret = model.carets[i];
            //    auto & Caret = i < carets.size() ? carets(i) : carets.emplace_back();
            //
            //    Caret.insert_mode = insert_mode.now || !caret.selection.empty();
            //
            //    auto color = i == 0 ?
            //        schemas[""].touched.back_color:
            //        schemas[""].focus.back_color;
            //    color.a = 192;
            //    Caret.canvas.color = color;
            //    Caret.coord = view.position(caret.position);
            //    Caret.insert_mode = insert_mode.now;
            //}
            //carets.truncate(model.carets.size());

            notify();
        }

        enum WHERE { THERE = 0,
             GLYPH, LINE, LINE_BEGIN, LINE_END, PAGE_TOP,
             TOKEN, PAGE, TEXT_BEGIN, TEXT_END, PAGE_BOTTOM,
        };

        void go (int where, bool selective = false)
        {
            // XYWH r = model.current_glyph();
            // XY p = r.origin;
            // switch (where) {
            // case THERE: break;
            // case GLYPH: 
            // case TOKEN:
            // case LINE: case LINE_BEGIN: case LINE_END: case PAGE_TOP:
            // case PAGE: case TEXT_BEGIN: case TEXT_END: case PAGE_BOTTOM:
            // }
            // model.go(p, selective);
        }

        void show (int where)
        {
        }

        void erase (int where = GLYPH)
        {
            // str s = model.selection();
            // if (where == GLYPH) model.insert("", false);
            // if (where == -GLYPH)
            // if (where == LINE)
            // model.erase(); refresh();
        }

        void insert (str s)
        {
            // str s = model.selection();
            // if (s == "\t")
            // if (s == "shift+\t")
            // 
            // model.insert(s, insert_mode.now);
            // 
            // refresh();
        }

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

            if (key == "alt+shift+up")
            {
                // int n = model.carets.size();
                // if (n >= 2 &&
                //     model.carets[n-2].position.line == 
                //     model.carets[n-1].position.line + 1)
                //     model.carets.truncate();
                // else model.carets += model::caret {
                //     doc::place{model.carets[n-1].position.line - 1},
                //     doc::range{}
                // };
            }
            if (key == "alt+shift+down")
            {
            }

            key.replace_all("alt+shift+", "shift+");

            // horizontal: 1 glyph, 2 token, 3 

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
            if (key == "ctrl+insert"      ) { sys::clipboard::set(model.selection()); } else
            if (key == "ctrl+shift+insert") {} else // VS: clipboard contex menu

            if (key == "delete"           ) { erase(); } else
            if (key == "shift+delete"     ) { sys::clipboard::set(model.selection()); erase(); } else
            if (key == "ctrl+delete"      ) { erase(LINE); } else // != VS
            if (key == "ctrl+shift+delete") {} else

            if (key == "enter"            ) { insert("\n"); } else
            if (key == "shift+enter"      ) { insert("\n"); } else
            if (key == "ctrl+enter"       ) { go(LINE_BEGIN); insert("\n"); } else
            if (key == "ctrl+shift+enter" ) { go(LINE_END  ); insert("\n"); } else

            if (key == "backspace"        ) { erase(-GLYPH); } else
            if (key == "alt+backspace"    ) { model.undo(); refresh(); } else
            if (key == "ctrl+backspace"   ) { model.redo(); refresh(); } else // != VS

            if (key == "tab"              ) { insert("\t"); } else
            if (key == "shift+tab"        ) { insert("shift+\t"); } else
            if (key == "escape"           ) { go(THERE); } else

            {
                insert(" [" + key + "] "); //(down ? " (down) " : " (up) ");
            }
        }
        void on_keyboard_input (str symbol) override
        {
            if (!touch) insert(symbol);
        }
        void on_focus (bool on) override
        {
            for (auto & caret : carets)
                caret.show(on);
        }

        bool touch = false;  XY touch_point; time touch_time;

        bool mouse_sensible (XY p) override { return true; }

        void on_mouse_wheel (XY p, int delta) override
        {
            if (delta < 0) while (delta++) show(-LINE);
            if (delta > 0) while (delta--) show(+LINE);
        }
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
                    touch_point = p;
                    touch_time = time::now;
                    //go(model.position(p + offset.now));
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
                //if (touch) go(model.position(p + offset.now), true);
            }
        }
    };
} 
