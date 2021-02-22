#pragma once
#include "doc_text_model.h"
#include "doc_ae_lexica.h"
#include "doc_ae_syntax_analysis.h"
namespace doc::ae
{
    using doc::text::report;

    struct model : doc::text::model
    {
        using base = doc::text::model;

        syntax::analysis::data syntax;

        ~model() { syntax::analysis::remove(path); }

        report log () override { return syntax.log; }

        void tokenize () override
        {
            tokens =
            lexica::parse(*this);
            syntax::analysis::pass1(path, syntax, tokens);
            syntax::analysis::pass2(path, syntax);
        }

        bool insert (str s) override
        {
            auto [c1, c2] = [this]() -> std::pair<str,str>
            {
                str s1, s2;
                for (auto [from, upto] : selections)
                {
                    if (from != upto) return {"",""};
                    auto [line, offset] = from;

                    if (offset < 2) return {"",""};
                    if (offset > lines[line].size()) return {"",""};
                    str c1 = lines[line][offset-2];
                    str c2 = lines[line][offset-1];

                    if (s1 == "") s1 = c1; else if (s1 != c1) return {"",""};
                    if (s2 == "") s2 = c2; else if (s2 != c2) return {"",""};
                }
                return {s1, s2};
            }
            ();

            if (c1 == "." && c2 == "." && s == ".") { base::insert(".");
                for (auto & sel : selections) sel.from.offset -= 3;
                base::insert((char*)(u8"…"));
                return true;
            }

            auto c = [this]() -> str
            {
                str s;
                for (auto [from, upto] : selections)
                {
                    if (from != upto) return "";
                    auto [line, offset] = from;

                    if (offset < 1) return "";
                    if (offset > lines[line].size()) return "";
                    str c = lines[line][offset-1];

                    if (s == "") s = c; else if (s != c) return "";
                }
                return s;
            }
            ();

            auto replace = [this, c, s](str c1, str c2, const char8_t* cc) {
                if (c != c1 || s != c2) return false;
                base::insert(s); for (auto & sel : selections) sel.from.offset -= 2;
                base::insert((char*)(cc));
                return true;
            };

            return
            replace("-",">", u8"→") ||
            replace("<","-", u8"←") ||
            replace("!","=", u8"≠") ||
            replace("<","=", u8"≤") ||
            replace(">","=", u8"≥") ||
            replace("~","=", u8"≈") ||
            replace("*","*", u8"×") ||
            base::insert(s);
        }
    };
}