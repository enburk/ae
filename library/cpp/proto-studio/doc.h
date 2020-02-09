#pragma once
#include <cassert>
#include "../aux_string.h"
namespace doc
{
    using namespace aux;

    struct place
    {
        int line = 0, offset = 0; // bool operator <=> (place p) const = default;
        bool operator == (place p) const { return line == p.line && offset == p.offset; }
        bool operator != (place p) const { return line != p.line || offset != p.offset; }
        bool operator <= (place p) const { return line <  p.line || offset <= p.offset && line == p.line; }
        bool operator <  (place p) const { return line <  p.line || offset <  p.offset && line == p.line; }
        bool operator >= (place p) const { return line >  p.line || offset >= p.offset && line == p.line; }
        bool operator >  (place p) const { return line >  p.line || offset >  p.offset && line == p.line; }
    };

    struct range { place from, upto; bool empty () const { return from == upto; } };

    struct glyph { str text; };

    struct text
    {
        array<array<glyph>> lines = {array<glyph>{glyph{"\n"}}};

        struct replace { range range; str text; };
        array <replace> undos;
        array <replace> redos;
        range perform (replace replace, int dir = 1 )
        {
            return range{};
        }
        range undo () { if (undos.empty()) return range{}; auto undo = undos.back(); undos.pop_back(); return perform(undo,-1); }
        range redo () { if (redos.empty()) return range{}; auto redo = redos.back(); redos.pop_back(); return perform(redo, 1); }

        str fragment (range range)
        {
            str s;
            assert(range.from <= range.upto);
            assert(range.upto.line < lines.size());
            assert(range.upto.offset <= lines[range.upto.line].size());
            for (place place = range.from; place < range.upto; )
            {
                s += lines[place.line][place.offset++].text;
                if (place.offset == lines[place.line].size()) {
                    place.offset = 0;
                    place.line++;
                }
            }
            return s;
        }
    };

    struct token
    {
        str text, kind; place place; // bool operator == (token t) const = default;
        bool operator != (const token & t) const { return !(*this == t); }
        bool operator == (const token & t) const { return
            text  == t.text &&
            kind  == t.kind &&
            place == t.place;
        }
    };

    struct document
    {
        text text;
    };

    struct line
    {
        array<glyph> glyphs = {glyph{"\n"}};
        array<token> tokens = {token{"\n", "", place{}}};
        bool updated = true;
    };

    struct caret { place position, selection; };

    struct entity { str kind, name; array<token> head, tail; array<entity> body; };

    struct Document
    {
        str type = "text";

        place begin() { return place{}; }
        place end  () { return place{lines.size()-1,
                       lines.back().glyphs.size()-1}; }

        array<line> lines = {line{}};

        array<caret> carets = {caret{}};

        array<entity> code;

        struct Command { str kind; place from, to; str text; };
        array <Command> undos; void undo ();
        array <Command> redos; void redo ();
        void perform (Command, int dir = 1);

        void upload  (str s)            { perform(Command{"insert", end(), end(), std::move(s)}, 0); } // w/o undo
        void append  (str s)            { perform(Command{"insert", end(), end(), std::move(s)}); }
        void insert  (place f, str s)          { perform(Command{"insert",  f, f, std::move(s)}); }
        void replace (place f, place l, str s) { perform(Command{"replace", f, l, std::move(s)}); }
        void remove  (place f, place l)        { perform(Command{"remove",  f, l, ""}); }
    };
}


  