#pragma once
#include "doc.h"
#include "doc_lexica_txt.h"
namespace doc
{
    struct text_model : text
    {
        array<range> selections;

        array<token> tokens;

        explicit text_model (str s = "") : text (s)
        {
            if (lines.size() == 0)
            lines += array<glyph>{"\n"};
            selections = array<range>{range{}};
            tokens = doc::lexica::txt::parse(*this);
        }

        struct replace
        {
            range range;
            text  text;
        };
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
                min(b.range.from, b.range.upto);
                });

            reundo undo;
            undo.selections = selections;
            selections.clear();

            for (auto && r : replaces)
            {
                auto & [from, upto] = r.range;
                from = aux::clamp(from, front(), back());
                upto = aux::clamp(upto, front(), back());
                if (from == upto && r.text == text{}) continue;
                if (from >= upto) std::swap(from, upto);

                undo.replaces += replace{range{}, text{}};
                undo.replaces.back().text.lines.reserve(upto.line - from.line + 1);

                for (place p = from; p < upto; p.offset = 0, p.line++)
                {
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
                    lines[upto.line].insert(upto.offset, line);
                    upto.offset += line.size();
                }

                undo.replaces.back().range = r.range;

                selections += range{upto, upto};
            }

            if (undo.replaces.size() == 0) return false;

            if (dir == 1) undoes += undo;
            if (dir ==-1) redoes += undo;

            tokens = doc::lexica::txt::parse(*this);

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
            
            for (range selection : selections)
            {
                range range = selection;
                auto & [from, upto] = range;
                auto & [line, offset] = from < upto ? upto : from;
                if (from != upto) {;} else
                if (offset >= lines[line].size() && line >= lines.size()-1) return false; else
                if (offset >= lines[line].size()) { line++; offset = 0; } else
                    offset++;

                replaces += replace{range, text{}};
            }

            return perform(replaces);
        }

        bool backspace ()
        {
            redoes.clear();

            array<replace> replaces;
            
            for (range selection : selections)
            {
                range range = selection;
                auto & [from, upto] = range;
                auto & [line, offset] = from < upto ? from : upto;
                if (from != upto) {;} else
                if (offset == 0 && line == 0) return false; else
                if (offset == 0) { line--; offset = lines[line].size(); } else
                    offset--;

                replaces += replace{range, text{}};
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
