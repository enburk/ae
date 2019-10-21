#pragma once
#include "pix.h"
#include "doc.h"
#include "doc_lexica_txt.h"
#include "gui_widget.h"
namespace gui
{
    struct Glyph : widget<Glyph>
    {
        //Glyph (base::widget* parent = nullptr) : widget (parent) {}

        //property<str> text;

        canvas canvas;

        //pix::glyph<RGBA> glyph;

        void on_render () {}
    };

    struct Token : widget<Token>
    {
        //Token (base::widget* parent = nullptr) : widget (parent) {}

        //property<str> text;

        pix::GLYPH<RGBA> glyph;

        void on_render () {}
    };

    struct TextLine : widget<Glyph>
    {
        //TextLine (base::widget* parent = nullptr) : widget (parent) {}

        //property<str> text; int top = 0;


        array<Glyph> glyphs;

        void on_render () {}
    };

    struct Text : widget<Text>
    {
        struct Context
        {
            Document* document; int top = 0;
        };

        Document internal_document;
        Context  internal_context {&internal_document};
        Context* context = &internal_context;

        //Text (base::widget* parent = nullptr) : widget (parent) {}

        array<TextLine> lines;

        void operator = (str s)
        {
        }

        void insert (str text) { insert (std::move(text), context->document->text.size()); }
        void insert (str text, int pos)
        {
            context->document->insert(text, pos);

            for (auto token : context->document->syntax)
            {

            }
        }

        void remove (int pos)
        {
        }


        void on_render () {}

        void on_change () override
        {
            if (coord.was.size != coord.now.size)
            {
                //auto r = coord.now.local();
                //auto h = r.h / 
                //text       .move_to(r);
            }
        }
    };
} 