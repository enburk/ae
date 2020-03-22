#pragma once
#include "doc.h"
namespace doc
{

    struct text_ // with the 'virtual space' support
    {
        place begin () const { return place{}; }
        place end   () const { return place{lines.size()-1, lines.back().size()}; }

        place front () const { return place{}; }
        place back  () const { return place{lines.size()-1, lines.back().size()}; }

        array<array<glyph>> lines = {{}}; // at least one line is always present

        array<array<glyph>> cut (range range)
        {
        }

        array<array<glyph>> copy (range range)
        {
            auto & [from, upto] = range;
            from = aux::clamp(front(), back());
            upto = aux::clamp(front(), back());
            if (from < upto) std::swap(from, upto);

            array<array<glyph>> copy; copy.reserve(upto.line - from.line + 1);

            for (; from < upto; from.offset = 0, from.line++)
            {
                copy += from.line == upto.line || from.offset > 0 ?
                    lines[from.line].from(from.offset).upto(upto.offset) :
                    lines[from.line];
            }
            return copy;
        }

        struct replace { range range; array<array<glyph>> text; };
        array <replace> undoes;
        array <replace> redoes;
        range undo () { if (undoes.empty()) return range{}; auto undo = undoes.back(); undoes.pop_back(); return perform(undo,-1); }
        range redo () { if (redoes.empty()) return range{}; auto redo = redoes.back(); redoes.pop_back(); return perform(redo, 1); }
        range perform (replace replace, int direction = 2 )
        {

            return replace.range;

            /*
            str erase = fragment(replace.range);
            if (erase == "" && replace.text == "") return range{};

            auto & [from, upto] = replace.range;
            
            if (from < upto) std::swap(from, upto);

            from.line = aux::clamp(from.line, 0, lines.size());
            upto.line = aux::clamp(upto.line, 0, lines.size());

            from.offset = aux::clamp(from.offset, 0, aux::max<int>());
            upto.offset = aux::clamp(upto.offset, 0, aux::max<int>());

            if (upto < from) upto = from; // possible if both were outside 

            if (from.line == upto.line)
            {
                if (lines.size() > from.line &&
                    lines[from.line].size() > from.offset)
                    lines[from.line].erase  ( from.offset, aux::min(upto.offset - from.offset,
                    lines[from.line].size() - from.offset));

                // lines += 
            }
            else // from.line < upto.line <= lines.size()
            {
                if (lines[from.line].size() > from.offset)
                    lines[from.line].erase  ( from.offset,
                    lines[from.line].size() - from.offset);

                int offset = lines[from.line].size();
                
                while (lines[from.line].size() < from.offset) lines[from.line] += glyph{" "};

                if (lines.size() > upto.line &&
                    lines[upto.line].size() > upto.offset)
                    lines[from.line].insert ( from.offset, 
                    lines[upto.line].from   ( upto.offset));


                if (lines.size() > upto.line)
                    lines[upto.line].erase(0, aux::min(upto.offset,
                    lines[upto.line].size()));


                from.offset = offset;
            }

            upto = from;




            int whole_lines = upto.line - from.line - 1;
            if (whole_lines > 0) {
                lines.erase(from.line + 1, whole_lines);
                upto.line -= whole_lines;
            }

            if (from.line < lines.size() &&
                from.offset < lines[from.line].size())
                lines[from.line].erase(from.offset, aux::min(
                    lines[from.line].size() - from.offset,
                    upto.line > from.line ? aux::max<int>() :
                    upto.offset - from.offset));

            if (from.line < upto.line)
            {
                lines[from.line] += lines[upto.line].from(upto.offset);
                lines.erase(upto.line);
            }



            while (from.line >= lines.size()) lines += array<glyph>{};
            while (from.offset >= lines[from.line].size()) lines[from.line] += glyph{" "};

            if (lines[from.line].size() == 0 ||
                lines[from.line].back().text != "\n") {
            }









            range.upto = range.from;

            str t; text += "\n";
            for (char c : text)
            {
                if (t.size () == 0
                ||  t.size () == 1 && (static_cast<uint8_t>(t[0]) & 0b11000000) == 0b11000000 // UTF-8: 110xxxxx 10xxxxxx
                ||  t.size () == 2 && (static_cast<uint8_t>(t[0]) & 0b11100000) == 0b11100000 // UTF-8: 1110xxxx 10xxxxxx 10xxxxxx
                ||  t.size () == 3 && (static_cast<uint8_t>(t[0]) & 0b11110000) == 0b11110000 // UTF-8: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                )   t += c; else { glyphs += glyph(t, style); t = c; }
            }

            replace.text  = erase;
            replace.range = range;
            if (direction >= 2) redoes.clear();
            if (direction >= 1) undoes += replace;
            if (direction ==-1) redoes += replace;

            return range;
            */
        }

        /*
        str fragment (range range)
        {
            auto & [from, upto] = range;
            
            if (from < upto) std::swap(from, upto);

            from.line = aux::clamp(from.line, 0, lines.size());
            upto.line = aux::clamp(upto.line, 0, lines.size());

            from.offset = aux::clamp(from.offset, 0, from.line < lines.size() ? lines[from.line].size() : 0);
            upto.offset = aux::clamp(upto.offset, 0, upto.line < lines.size() ? lines[upto.line].size() : 0);

            str s;

            for (place place = from; place < upto; )
            {
                if (place.offset == lines[place.line].size()) {
                    place.offset = 0;
                    place.line++;
                    s += "\n";
                }
                else s += lines[place.line][place.offset++].text;
            }
            return s;
        }

        struct replace { range range; str text; };
        array <replace> undoes;
        array <replace> redoes;
        range undo () { if (undoes.empty()) return range{}; auto undo = undoes.back(); undoes.pop_back(); return perform(undo,-1); }
        range redo () { if (redoes.empty()) return range{}; auto redo = redoes.back(); redoes.pop_back(); return perform(redo, 1); }
        range perform (replace replace, int direction = 2 )
        {
            str erase = fragment(replace.range);
            if (erase == "" && replace.text == "") return range{};

            auto & [from, upto] = replace.range;
            
            if (from < upto) std::swap(from, upto);

            from.line = aux::clamp(from.line, 0, lines.size());
            upto.line = aux::clamp(upto.line, 0, lines.size());

            from.offset = aux::clamp(from.offset, 0, aux::max<int>());
            upto.offset = aux::clamp(upto.offset, 0, aux::max<int>());

            if (upto < from) upto = from; // possible if both were outside 

            if (from.line == upto.line)
            {
                if (lines.size() > from.line &&
                    lines[from.line].size() > from.offset)
                    lines[from.line].erase  ( from.offset, aux::min(upto.offset - from.offset,
                    lines[from.line].size() - from.offset));

                // lines += 
            }
            else // from.line < upto.line <= lines.size()
            {
                if (lines[from.line].size() > from.offset)
                    lines[from.line].erase  ( from.offset,
                    lines[from.line].size() - from.offset);

                int offset = lines[from.line].size();
                
                while (lines[from.line].size() < from.offset) lines[from.line] += glyph{" "};

                if (lines.size() > upto.line &&
                    lines[upto.line].size() > upto.offset)
                    lines[from.line].insert ( from.offset, 
                    lines[upto.line].from   ( upto.offset));


                if (lines.size() > upto.line)
                    lines[upto.line].erase(0, aux::min(upto.offset,
                    lines[upto.line].size()));


                from.offset = offset;
            }

            upto = from;




            int whole_lines = upto.line - from.line - 1;
            if (whole_lines > 0) {
                lines.erase(from.line + 1, whole_lines);
                upto.line -= whole_lines;
            }

            if (from.line < lines.size() &&
                from.offset < lines[from.line].size())
                lines[from.line].erase(from.offset, aux::min(
                    lines[from.line].size() - from.offset,
                    upto.line > from.line ? aux::max<int>() :
                    upto.offset - from.offset));

            if (from.line < upto.line)
            {
                lines[from.line] += lines[upto.line].from(upto.offset);
                lines.erase(upto.line);
            }
        }



            while (from.line >= lines.size()) lines += array<glyph>{};
            while (from.offset >= lines[from.line].size()) lines[from.line] += glyph{" "};

            if (lines[from.line].size() == 0 ||
                lines[from.line].back().text != "\n") {
            }









            range.upto = range.from;

            str t; text += "\n";
            for (char c : text)
            {
                if (t.size () == 0
                ||  t.size () == 1 && (static_cast<uint8_t>(t[0]) & 0b11000000) == 0b11000000 // UTF-8: 110xxxxx 10xxxxxx
                ||  t.size () == 2 && (static_cast<uint8_t>(t[0]) & 0b11100000) == 0b11100000 // UTF-8: 1110xxxx 10xxxxxx 10xxxxxx
                ||  t.size () == 3 && (static_cast<uint8_t>(t[0]) & 0b11110000) == 0b11110000 // UTF-8: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                )   t += c; else { glyphs += glyph(t, style); t = c; }
            }

            replace.text  = erase;
            replace.range = range;
            if (direction >= 2) redoes.clear();
            if (direction >= 1) undoes += replace;
            if (direction ==-1) redoes += replace;

            return range;
        }
        */
    };
}
//        auto && [kind, from, to, s] = command;
//
//        if (pos == text.size()) nn = 0; // allow "replace" at the end of text
//
//        if (kind == "insert"  && s == "") return;
//        if (kind == "replace" && s == "") kind = "remove";
//        if (kind == "replace" && nn == 0) kind = "insert";
//        if (kind == "remove"  && nn == 0) return;
//
//        if (pos + nn > text.size()) throw std::out_of_range ("Document:"
//        "\n size = " + std::to_string(text.size()) +
//        "\n pos  = " + std::to_string(pos) +
//        "\n nn   = " + std::to_string(nn));
//
//        if (dir != 0)
//        {
//            auto & dos = dir == 1 ? undos : redos; int n = s.size();
//            if (kind == "insert" ) dos += Command{"remove",  pos, n, ""}; else
//            if (kind == "replace") dos += Command{"replace", pos, n, str(text.interval(pos, nn))}; else
//            if (kind == "remove" ) dos += Command{"insert",  pos, 0, str(text.interval(pos, nn))};
//        }
//
//        if (kind == "insert" ) text.insert (pos, s.begin(), s.end()); else
//        if (kind == "replace") text.replace(pos, nn, s.begin(), s.end()); else
//        if (kind == "remove" ) text.erase  (pos, nn);
