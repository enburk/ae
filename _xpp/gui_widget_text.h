#pragma once
#include "sys.h"
#include "doc.h"
#include "doc_lexica_txt.h"
#include "gui_widget.h"
#include "gui_widgetarium.h"
#include "gui_widget_layout.h"
namespace gui
{
    struct text
    {
        struct glyph:
        widget<glyph>
        {
            unary_property<sys::glyph> sys_glyph;

            Opacity opacity () override { return semitransparent; }

            void on_change (void* what) override { if (what != &sys_glyph) return;
                advance = XY(sys_glyph.now.advance, 0);
                baseline = XY(0, sys_glyph.now.ascent);
                resize(sys_glyph.now.size);
                update();
            }
            void on_render (Frame<RGBA> frame, XY offset, uint8_t alpha) override {
                sys::render(sys_glyph.now, frame, offset, alpha, coord.now.x);
            }
        };

        struct token final : widget<token>
        {
            unary_property<sys::token> sys_token;

            widgetarium<glyph> glyphs;

            void on_change (void* what) override { if (what != &sys_token) return;
                for (int i = 0; i < sys_token.now.glyphs.size(); i++) {
                    const auto & sys_glyph = sys_token.now.glyphs[i];
                    glyph & glyph = i < glyphs.size() ? glyphs(i) : glyphs.emplace_back();
                    glyph.sys_glyph = sys_glyph;
                    glyph.move_to(sys_token.now.offsets[i]);
                }
                glyphs.resize(sys_token.now.size);
                resize(sys_token.now.size);
            }
        };

        ///////////////////////////////////////////////////////////////////////

        template<orientation> struct align;
        template<> struct align<vertical> { enum type { top, center, bottom }; };
        template<> struct align<horizontal> { enum type { left, center, right, justify}; };

        struct line final : widgetarium<token>
        {
            void fill (str text, sys::glyph_style style) {
                array<doc::Token> tokens = doc::lexica::txt(text);
                std::map<str, sys::glyph_style> styles;
                fill (max<int>(), false, tokens.range(), styles, style);
            }
            void fill (
                int width,
                bool word_wrap,
                Range<doc::Token> tokens,
                const std::map<str, sys::glyph_style> & styles,
                const sys::glyph_style & default_style)
            {
                if (width == 0) return;

                struct row
                {
                    int ascent  = 0;
                    int descent = 0;
                    int advance = 0;
                    array<int> offsets;
                    XYWH coord;
                };
                array<row> rows; rows += row();

                for(auto [t, n] : tokens)
                {
                    token & token = n < size() ? (*this)(n) : emplace_back();
                    sys::glyph_style style;
                    auto it = styles.find(t.kind);
                    style = it != styles.end() ? it->second : default_style;
                    token.sys_token = sys::token(t.text, style); // could be optimized
                    if (rows.back().coord.size.x + rows.back().advance + token.sys_token.now.size.x > width) rows += row();
                    auto & r = rows.back();
                    r.ascent  = max (r.ascent,  token.sys_token.now.ascent);
                    r.descent = max (r.descent, token.sys_token.now.descent);
                    r.offsets += r.coord.size.x + r.advance;
                    r.advance = token.sys_token.now.advance;
                    r.coord.size.y = max (r.coord.size.y, r.ascent + r.descent);
                    r.coord.size.x = r.offsets.back() + token.sys_token.now.size.x;
                }
                width = 0; for (auto & r : rows) width = max (width, r.coord.size.x);
                int n = 0; for (auto & r : rows) for (int offset : r.offsets)
                {
                    token & token = (*this)(n++);
                    token.move_to(XY(r.coord.x + offset, r.coord.y + r.ascent - token.sys_token.now.ascent));
                }
                truncate(n);
                resize(XY(width, rows.back().coord.y + rows.back().coord.size.y));
            }
        };

        ///////////////////////////////////////////////////////////////////////
            
        struct page final : widgetarium<line>
        {
            void fill (
                int width,
                bool word_wrap,
                const array<doc::Token> & tokens,
                const std::map<str, sys::glyph_style> & styles,
                const sys::glyph_style & default_style)
            {
                if (width == 0) return;
                int l = 0;
                for (auto i = tokens.begin(); i != tokens.end(); ) {
                     auto j = std::find_if(i, tokens.end(), [](auto t){ return t.text == "\n"; });
                     if (j != tokens.end()) j++;
                     if (l >= size()) emplace_back();
                     (*this)(l++).fill(width, word_wrap, array<doc::Token>(i, j).range(), styles, default_style);
                     i = j;
                }
                truncate(l);
                int w = 0, h = 0;
                for (auto & line : *this) {
                    line.move_to(XY(0, h));
                    h += line.coord.now.size.y;
                    w = max (w, line.coord.now.size.x);
                }
                resize(XY(w,h));
            }
        };

        ///////////////////////////////////////////////////////////////////////

        struct label final : widget<label>
        {
            canvas canvas; page page;

            binary_property<str> text;
            binary_property<str> html;

            property<RGBA> color;
            binary_property<sys::glyph_style> style;

            binary_property<bool> word_wrap = true;
            binary_property<
                align<horizontal>::type> alignment =
                align<horizontal>::center;

            array<doc::Token> tokens;
            std::map<str, sys::glyph_style> styles;

            void refresh ()
            {
                page.fill (coord.now.size.x, word_wrap.now, tokens, styles, style.now);

                if (alignment.now == align<horizontal>::center)
                    page.move_to(coord.now.size/2 - page.coord.now.size/2);
            }

            void on_change (void* what) override
            {
                if (what == &text)
                {
                    tokens = doc::lexica::txt(text.now);
                    styles.clear();

                    html.was = html.now; html.now = "";
                    for (auto && token : tokens)
                    html.now += doc::html::encoded(token.text);

                    refresh();
                }
                if (what == &html)
                {
                //  page.clear();
                //  text.was = text.now;
                //  for (auto && token : browser(
                //      doc::translator::html(html))) {
                //      text.now += token.text;
                //      page.append(token);
                //  }
                }
                if (what == &coord && coord.was.size != coord.now.size)
                {
                    canvas.coord = coord.now.local();
                    if (style.now == sys::glyph_style())
                        style = sys::glyph_style{ sys::font{"", metrics::text::height}, pix::black };
                    refresh();
                }
                if (what == &color)
                {
                    style.was = style.now;
                    style.now.color = color.now;
                    refresh();
                }
                if (what == &style)
                {
                    color.was = color.now;
                    color.now = style.now.color;
                    refresh();
                }
                if (what == &word_wrap)
                {
                    refresh();
                }
                //if (what == &align)
                //{
                //}
            }
        };

        ///////////////////////////////////////////////////////////////////////

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
            label view; // after selection, before carets
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
                s += key; view.text = s;
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



    };
} 