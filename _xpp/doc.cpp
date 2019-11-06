#include "doc.h"
#include "doc_lexica_txt.h"
namespace doc
{
    static void proceed (Document & doc, Range<char> range)
    {
        //lexica = lexica::txt(text.range(pos, s.size()));
    }

    void Document::insert  (str s) { insert (std::move(s), text.size()); }
    void Document::insert  (str s, int pos)
    {
        if (s == "") return;

        if (pos > text.size()) throw std::out_of_range ("Document::insert: "
            " size = " + std::to_string(text.size()) +
            ", pos = " + std::to_string(pos));

        undo += Command { "remove", pos, s.size(), "" };

        text.insert(text.begin() + pos, s.begin(), s.end());

        proceed(*this, text.range(pos, s.size()));
    }

    void Document::remove  (int pos, int n)
    {
    }

    void Document::replace (int pos, int n, str s)
    {
        if (n ==  0) { insert (s, pos); return; }
        if (s == "") { remove (pos, n); return; }

    }
}
