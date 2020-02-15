#pragma once
#include <filesystem>
#include "aux_string.h"
#include "aux_utils.h"
#include "pix_color.h"
#include "pix_image.h"
namespace sys
{
    using pix::XY;
    using pix::XYWH;
    using pix::XYXY;
    using pix::RGBA;
    using pix::MONO;
    using namespace aux;

    namespace screen
    {
        inline XY size;
    }

    namespace window
    {
        inline pix::image<RGBA> image;
        void update ();
        void timing ();
        namespace on
        {
            void start   ();
            void finish  ();
            void turn_on ();
            void turn_off();
            void pause   ();
            void resume  ();
            void timing  ();
            void resize  ();
        }
    }

    namespace mouse
    {
        void cursor(str);
        namespace on
        {
            void move  (XY);
            void wheel (XY, int);
            void press (XY, char, bool);
            void leave ();
        }
    }

    namespace keyboard
    {
        namespace on
        {
            void focus (bool);
            void press (str, bool);
            void input (str);
        }
    }

    namespace clipboard
    {
        void set (str);
        void set (pix::frame<RGBA>);
        namespace get {
            pix::image<RGBA> image ();
            str string ();
        }
    }

    namespace settings
    {
        str  load (str name, str default_value);
        int  load (str name, int default_value);
        void save (str name, str value);
        void save (str name, int value);
    }

    struct app_base : polymorphic
    {
        str title;
        virtual void constructor() = 0;
        virtual void destructor () = 0;
    };
    struct app_instance
    {
        static void init();
        static void done();
        static app_base * app;
        app_instance () { init(); if (app) app->constructor(); }
       ~app_instance () { if (app) app->destructor (); done(); }
    };
    template<class APP> struct app : app_base
    {
        APP * apptr = nullptr;;
        app (str s) { app_instance::app = this; title = s; }
        void constructor() override { apptr = new APP; }
        void destructor () override { delete apptr; }
    };
    inline app_base * app_instance::app = nullptr;

    struct font
    {
        str face; int size; bool bold; bool italic;

        font(str face = "", int size = 0, bool b=false, bool i=false)
            : face(face), size(size), bold(b), italic(i) {}

        bool operator != (const font & f) const { return ! (*this == f); }
        bool operator == (const font & f) const { return
            face   == f.face &&
            size   == f.size &&
            bold   == f.bold &&
            italic == f.italic; }

        struct metrics
        {
            int height;  // ascent + descent
            int ascent;  // units above the base line
            int descent; // units below the base line (positive value)
            int linegap; // baseline-to-baseline distance = ascent + descent + linegap
            int average_char_width; // usually the width of 'x'
            int maximum_char_width;
            int minimum_char_width;
        };
    };

    struct glyph_style
    {
        struct line { str style; int width = 0; RGBA color;
        bool operator != (const line & l) const { return ! (*this == l); }
        bool operator == (const line & l) const { return
            style == l.style &&
            width == l.width &&
            color == l.color; }
        };

        font font;
        RGBA color;
        RGBA background;
        line underline;  // "solid", "double", "dahsed", "dotted", "wavy"
        line strikeout;
        line outline;

        bool operator != (const glyph_style & s) const { return ! (*this == s); }
        bool operator == (const glyph_style & s) const { return
            font      == s.font       &&
            color     == s.color      &&
            background== s.background &&
            underline == s.underline  &&
            strikeout == s.strikeout  &&
            outline   == s.outline; }
    };

    struct glyph_metrics
    {
        XY  size;        // size.y = ascent + descent
        int ascent  = 0; // units above the base line
        int descent = 0; // units below the base line (positive value)
        int advance = 0; // the pen position increment = size.x + advance

        bool operator != (const glyph_metrics & m) const { return ! (*this == m); }
        bool operator == (const glyph_metrics & m) const { return
            size    == m.size    &&
            ascent  == m.ascent  &&
            descent == m.descent &&
            advance == m.advance; }
    };

    inline array<sys::glyph_style> glyph_styles;
    inline int style_index (const glyph_style & style) {
        return (int)(glyph_styles.find_or_emplace(style) -
                     glyph_styles.begin());
    }

    struct glyph : glyph_metrics
    {
        str text; int style_index;

        explicit glyph (str, glyph_style);
        explicit glyph () = default;

        glyph_style style () const { return glyph_styles[style_index]; }

        bool operator != (const glyph & g) const { return ! (*this == g); }
        bool operator == (const glyph & g) const { return text == g.text &&
            style_index == g.style_index; }
    };

    struct token : glyph
    {
        array<glyph> glyphs; array<XY> offsets;

        explicit token (str, glyph_style);
        explicit token () = default;
    };

    font::metrics metrics (font);
    void render (glyph, pix::frame<RGBA>, XY offset=XY(), uint8_t alpha=255, int x=0);
    void render (token, pix::frame<RGBA>, XY offset=XY(), uint8_t alpha=255, int x=0);
}
namespace pix
{
    expected<pix::image<RGBA>> read (std::filesystem::path);
    expected<nothing> write (pix::frame<RGBA>, std::filesystem::path, int quality = -1);
    expected<array<std::byte>> pack (pix::frame<RGBA>, str format, int quality = -1);
    expected<pix::image<RGBA>> unpack (array<std::byte>::range);
    expected<pix::image<RGBA>> unpack (void* buffer, int size);
}
