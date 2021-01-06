#pragma once
#include "doc_text_text.h"
namespace doc::text
{
    struct model : text
    {
        array<range> selections;

        explicit model (str s = "") : text(s)
        {
            for (auto & line : lines) line.trimr(glyph(" "));
            if (lines.size() == 0) lines += array<glyph>{};
            selections = array<range>{range{}};
        }

        struct replace { range range; text text; };

        struct reundo
        {
            array<replace> replaces;
            array<range> selections;
        };
        array<reundo> undoes;
        array<reundo> redoes;

        place front () const { return place{}; }
        place back  () const { return place{lines.size()-1, lines.back().size()}; }

        bool perform (array<replace> replaces, int dir = 1 )
        {
            replaces.sort([](auto a, auto b){ return
                min(a.range.from, a.range.upto) >
                min(b.range.from, b.range.upto); });

            reundo undo;
            undo.selections = selections;
            selections.clear();

            for (auto && r : replaces)
            {
                auto & [from, upto] = r.range;
                place last = back(); last.offset = max<int>();
                from = clamp(from, front(), last);
                upto = clamp(upto, front(), last);
                if (from > upto) std::swap(from, upto);
                if (from == upto && r.text == text{}) {
                    selections += range{from, upto};
                    continue;
                }

                undo.replaces += replace{range{}, text{}};
                undo.replaces.back().text.lines.reserve(upto.line - from.line + 1);

                for (place p = from; p <= upto; p.offset = 0, p.line++) {
                    undo.replaces.back().text.lines += p.line == upto.line ?
                        lines[p.line].from(p.offset).upto(upto.offset) :
                        lines[p.line].from(p.offset);
                }

                if (from.line == upto.line)
                    lines[from.line].from(from.offset).upto(upto.offset).erase();
                else {
                    lines[from.line].from(from.offset).erase();
                    lines[upto.line].upto(upto.offset).erase();
                    lines[from.line] += lines[upto.line];
                    lines.from(from.line+1)
                         .upto(upto.line+1)
                         .erase();
                }

                upto = from;
                int n = from.offset - lines[from.line].size();
                if (n > 0) { from.offset -= n; while (n--) lines[from.line] += " "; }

                bool first_line = true;
                for (auto && line : r.text.lines)
                {
                    if (first_line) first_line = false; else
                    {
                        lines.insert(upto.line+1,
                        lines[upto.line].from(upto.offset));
                        lines[upto.line].from(upto.offset).erase();
                        upto.offset = 0;
                        upto.line++;
                    }
                    lines[upto.line].insert(upto.offset, line.from(0));
                    upto.offset += line.size();
                }

                undo.replaces.back().range = r.range;

                selections += range{upto, upto};
            }

            if (undo.replaces.size() == 0) return false;

            if (dir == 1) undoes += undo;
            if (dir ==-1) redoes += undo;

            return true;
        }

        bool undo ()
        {
            if (undoes.empty()) return false;
            auto undo = undoes.back(); undoes.pop_back();
            perform(undo.replaces,-1);
            selections = undo.selections;
            return true;
        }
        bool redo ()
        {
            if (redoes.empty()) return false;
            auto redo = redoes.back(); redoes.pop_back();
            perform(redo.replaces, 1);
            selections = redo.selections;
            return true;
        }

        bool erase ()
        {
            redoes.clear();

            array<replace> replaces;
            
            for (auto [from, upto] : selections)
            {
                if (from == upto)
                {
                    auto & [line, offset] = upto; int n = lines[line].size();
                    if (offset >= n && line >= lines.size()-1) return false; else
                    if (offset >= n) { line++; offset = 0; } else
                        offset++;
                }

                replaces += replace{range{from, upto}, text{}};
            }

            return perform(replaces);
        }

        bool backspace ()
        {
            redoes.clear();

            array<replace> replaces;
            
            for (auto [from, upto] : selections)
            {
                if (from == upto)
                {
                    auto & [line, offset] = upto;
                    if (offset == 0 && line == 0) return false; else
                    if (offset == 0) { line--; offset = lines[line].size(); } else
                        offset--;
                }

                replaces += replace{range{from, upto}, text{}};
            }

            return perform(replaces);
        }

        bool insert (str s)
        {
            redoes.clear();

            array<replace> replaces;

            for (range selection : selections)
            {
                replaces += replace{selection, text(s)};
            }

            return perform(replaces);
        }
    };
}

#include "data_unittest.h"
namespace data::unittest
{
    void out (doc::text::model const& m)
    {
        for (auto & line : m.lines) out(doc::text::string(line));
    }

    void text_model () try
    {
        using doc::text::model;

        test("text.model.ctor");
        {
            oops( model m; out(m) ) { "" };
            oops( model m(" "); out(m) ) { "" };
            oops( model m("a"); out(m) ) { "a" };
            oops( model m("a "); out(m) ) { "a" };
            oops( model m(" a"); out(m) ) { " a" };
            oops( model m("\n"); out(m) ) { "", "", "-----" };
            oops( model m("\n "); out(m) ) { "", "", "-----" };
            oops( model m(" \n"); out(m) ) { "", "", "-----" };
            oops( model m(" \n "); out(m) ) { "", "", "-----" };
            oops( model m("a\nb"); out(m) ) { "a", "b", "-----" };
            oops( model m("a \nb"); out(m) ) { "a", "b", "-----" };
            oops( model m("a\n b"); out(m) ) { "a", " b", "-----" };
        }
        test("text.model.char");
        {
            model m;
            oops( m.insert(" "); out(m) ) { " " };
            oops( m.insert("a"); out(m) ) { " a" };
            oops( m.insert(" "); out(m) ) { " a " };
            oops( m.backspace(); out(m) ) { " a" };
            oops( m.erase    (); out(m) ) { " a" };
            oops( m.undo     (); out(m) ) { " a " };
            oops( m.undo     (); out(m) ) { " a" };
            oops( m.undo     (); out(m) ) { " " };
            oops( m.undo     (); out(m) ) { "" };
            oops( m.undo     (); out(m) ) { "" };
            oops( m.redo     (); out(m) ) { " " };
            oops( m.redo     (); out(m) ) { " a" };
            oops( m.redo     (); out(m) ) { " a " };
            oops( m.redo     (); out(m) ) { " a" };
            oops( m.redo     (); out(m) ) { " a" };
        }
        test("text.model.line");
        {
            model m;
            oops( m.insert("a"); out(m) ) { "a" };
            oops( m.insert("b"); out(m) ) { "ab" };
            oops( m.insert("c"); out(m) ) { "abc" };
            m.selections[0].from.offset--;
            m.selections[0].upto.offset--;
            oops( m.insert("d"); out(m) ) { "abdc" };
            oops( m.backspace(); out(m) ) { "abc" };
            oops( m.erase    (); out(m) ) { "ab" };
            oops( m.erase    (); out(m) ) { "ab" };
            m.selections[0].from.offset--;
            m.selections[0].upto.offset--;
            oops( m.backspace(); out(m) ) { "b" };
            oops( m.backspace(); out(m) ) { "b" };
            oops( m.insert("a"); out(m) ) { "ab" };
            oops( m.undo     (); out(m) ) { "b" };
            oops( m.undo     (); out(m) ) { "ab" };
            oops( m.undo     (); out(m) ) { "abc" };
            oops( m.undo     (); out(m) ) { "abdc" };
            oops( m.undo     (); out(m) ) { "abc" };
            oops( m.undo     (); out(m) ) { "ab" };
            oops( m.undo     (); out(m) ) { "a" };
            oops( m.undo     (); out(m) ) { "" };
            oops( m.undo     (); out(m) ) { "" };
        }
        test("text.model.redo");
        {
            model m;
            oops( m.redo     (); out(m) ) { "" };
            oops( m.undo     (); out(m) ) { "" };
            oops( m.insert("a"); out(m) ) { "a" };
            oops( m.redo     (); out(m) ) { "a" };
            oops( m.undo     (); out(m) ) { "" };
            oops( m.redo     (); out(m) ) { "a" };
            oops( m.backspace(); out(m) ) { "" };
            oops( m.redo     (); out(m) ) { "" };
            oops( m.undo     (); out(m) ) { "a" };
            oops( m.undo     (); out(m) ) { "" };
            oops( m.erase    (); out(m) ) { "" };
            oops( m.redo     (); out(m) ) { "" };
        }
    }
    catch(assertion_failed){}
}
