#pragma once
#include "doc_text_repo.h"
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

        void tokenize () override
        {
            syntax::analysis::abort(path);
            tokens = lexica::parse(*this);
            syntax::analysis::start(path, syntax, tokens);
        }

        void prereanalyze () override
        {
            syntax::analysis::prereanalize(path);
        }

        void reanalyze () override
        {
            syntax::analysis::reanalize(path);
        }

        void tick () override
        {
            if (syntax::analysis::tick(path) ==
                syntax::analysis::state::depend)
            {
                for (auto [name, path] : syntax.dependees)
                    doc::text::repo::load<model>
                        (path)->reanalyze();

                syntax::analysis::resume(path);
            }
        }

        bool ready () override { return
            syntax::analysis::tick(path) ==
            syntax::analysis::state::ready;
        }

        report log () override { // don't ask until it's ready
            report log;
            log += syntax.log1;
            log += syntax.log2;
            log += syntax.log3;
            return log;
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