#pragma once
#include "doc.h"
#include "doc_ae_lexica.h"
#include "doc_ae_syntax_analysis.h"
#include "doc_cpp_lexica.h"
#include "doc_cpp_syntax.h"
#include "doc_text_lexica.h"
namespace doc
{
    struct text_model : text
    {
        str format;
        array<range> selections;
        array<token> tokens;
        report log;

        explicit text_model (str t = "", str f = "txt") : text(t), format(f)
        {
            if (lines.size() == 0)
            lines += array<glyph>{"\n"};
            selections = array<range>{range{}};
            process();
        }

        void process ()
        {
            tokens = 
                format == "ae"  ? doc::ae::lexica ::parse(*this):
                format == "cpp" ? doc::lexica::cpp::parse(*this):
                                  doc::lexica::txt::parse(*this);

            log.messages.clear();

            if (format == "ae" ) { doc::ae ::syntax::analysis(log).proceed(tokens); }
            if (format == "cpp") { doc::cpp::syntax::analysis(log).proceed(tokens); }
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
                    lines[upto.line].insert(upto.offset, line);
                    upto.offset += line.size();
                }

                undo.replaces.back().range = r.range;

                selections += range{upto, upto};
            }

            if (undo.replaces.size() == 0) return false;

            if (dir == 1) undoes += undo;
            if (dir ==-1) redoes += undo;

            process();
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
            
            if (s == "\n" &&
                selections.size() == 1 &&
                selections[0].from == selections[0].upto &&
                selections[0].from.offset > 0)
            {
                int spaces = 0;
                str lead = " ";
                int line = selections[0].from.line;
                while (line >= 0)
                {
                    for (auto c : lines[line]) {
                        lead = c; if (c == " ")
                            spaces++; else
                            break;
                    }
                    if (lead != " ") break;
                    spaces = 0;
                    line--;
                }
                if (lead == "{" and spaces <= selections[0].from.offset) spaces += 4;
                else spaces = min(spaces, selections[0].from.offset);
                s += str(' ', spaces);

                replaces += replace{selections[0], text(s)};
            }
            else
            if (s == "\t" || s == "shift+\t")
            {
                bool case2 = false;
                for (auto [from, upto] : selections)
                    if (from != upto) case2 = true;

                if (not case2)
                {
                    for (auto [from, upto] : selections)
                    {
                        if (s == "\t")
                            replaces += replace{range{from, from},
                                text(str(' ', 4 - from.offset % 4))};

                        else {
                            int n = from.offset % 4;
                            if (n == 0) n = 4;
                            for (int i=0; i<n; i++)
                                if (from.offset > 0)
                                    if (lines[from.line].size() < from.offset ||
                                        lines[from.line][from.offset-1] == " ")
                                        from.offset--;

                            replaces += replace{range{from, upto}, text{}};
                        }
                    }
                }
                else
                {
                    std::map<int, int> spaces_by_line;

                    for (auto [from, upto] : selections)
                        for (place p = from; p < upto; p.offset = 0, p.line++)
                            for (int n=0; n<lines[p.line].size(); n++)
                                if (lines[p.line][n] != " ") break;
                                    else spaces_by_line[p.line] = n+1;

                    int spaces = 
                        spaces_by_line.size() == 0 ? 0 : std::min_element(
                        spaces_by_line.begin(),
                        spaces_by_line.end())
                        ->second; 

                    if (s == "shift+\t" && spaces == 0) return false;

                    reundo undo {.selections=selections};

                    for (auto & [from, upto] : selections)
                    {
                        for (place p = from; p < upto; p.offset = 0, p.line++)
                        {
                            if (s == "\t")
                            {
                                int n = 4 - spaces % 4;
                                array<glyph> ss; for (int i=0; i<n; i++) ss += " ";
                                lines[p.line].insert(0, ss);
                                from.offset += n;
                                upto.offset += n;
                                undo.replaces += replace{range{
                                    place{p.line, 0}, place{p.line, n}},
                                    text{}};
                            }
                            else if (s == "shift+\t")
                            {
                                int n = spaces % 4; if (n == 0) n = 4;
                                lines[p.line].upto(n).erase();
                                from.offset -= n; if (from.offset < 0) from.offset = 0;
                                upto.offset -= n; if (upto.offset < 0) upto.offset = 0;
                                undo.replaces += replace{range{
                                    place{p.line, 0}, place{p.line, 0}},
                                    text{str(' ', n)}};
                            }
                            else std::terminate();
                        }
                    }

                    undoes += undo;
                    process();
                    return true;
                }
            }
            else
            for (range selection : selections)
            {
                replaces += replace{selection, text(s)};
            }

            return perform(replaces);
        }
    };

}
