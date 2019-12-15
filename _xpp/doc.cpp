#include "doc.h"
#include "doc_lexica_txt.h"
namespace doc
{
    void Document::perform (Command command, int dir)
    {
        auto && [kind, pos, nn, s] = command;

        if (pos == text.size()) nn = 0; // allow "replace" at the end of text

        if (kind == "insert"  && s == "") return;
        if (kind == "replace" && s == "") kind = "remove";
        if (kind == "replace" && nn == 0) kind = "insert";
        if (kind == "remove"  && nn == 0) return;

        if (pos + nn > text.size()) throw std::out_of_range ("Document:"
        "\n size = " + std::to_string(text.size()) +
        "\n pos  = " + std::to_string(pos) +
        "\n nn   = " + std::to_string(nn));

        if (dir != 0)
        {
            auto & dos = dir == 1 ? undos : redos; int n = s.size();
            if (kind == "insert" ) dos += Command{"remove",  pos, n, ""}; else
            if (kind == "replace") dos += Command{"replace", pos, n, str(text.interval(pos, nn))}; else
            if (kind == "remove" ) dos += Command{"insert",  pos, 0, str(text.interval(pos, nn))};
        }

        if (kind == "insert" ) text.insert (pos, s.begin(), s.end()); else
        if (kind == "replace") text.replace(pos, nn, s.begin(), s.end()); else
        if (kind == "remove" ) text.erase  (pos, nn);

        //Token t; t.text = str(' ', nn); t.offset = pos;
        //auto f = lexica.lower_bound(t, [](const auto & i, const auto & v){ return i.offset + i.text.size() < v.offset; });
        //auto l = lexica.upper_bound(t, [](const auto & v, const auto & i){ return v.offset + v.text.size() < i.offset; });
        //if (f != lexica.begin()) --f;
        //if (l != lexica.end()) ++l;
        //if (f != lexica.end()) pos = f->offset; int n = text.size() - pos;
        //if (l != lexica.end()) n = l->offset + l->text.size() - pos + s.size() - nn;
        //lexica.replace (f, l,
        //    type == "ae"   ?  lexica::txt(text.range(pos, n)) :
        //    type == "c++"  ?  lexica::txt(text.range(pos, n)) :
        //    type == "html" ?  lexica::txt(text.range(pos, n)) :
        //                      lexica::txt(text.range(pos, n))
        //);
        lexica = lexica::txt(text.range(0, text.size()));
    }

    void Document::redo () { if (redos.size() == 0) return; perform(redos.back(), 1); redos.pop_back(); }
    void Document::undo () { if (undos.size() == 0) return; perform(undos.back(),-1); undos.pop_back(); }
}

