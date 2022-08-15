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

        ~model() { syntax::analysis::repo.erase(path); }

        auto& syntax () { return syntax::analysis::repo[path]; }

        void tokenize () override
        {
            syntax().abort();
            tokens = lexica::parse(*this);
            syntax().tokens = &tokens;
            syntax().path = path;
            syntax().start();
        }

        void reanalyze () override { syntax().reanalyze(); }
        bool ready () override { return syntax().ready(); }
        void tick () override
        {
            if (syntax().status == syntax::analysis::state::depend) {
                syntax().status =  syntax::analysis::state::check;

                for (auto [name, path]:
                syntax().dependencies.dependees)
                doc::text::repo::load<model>(path);
            }
            syntax::analysis::tick(path);
        }

        report log (array<std::filesystem::path>& visited) override
        {
            report log;
            log.path = path.string();
            log.trace (path.string()
                + light(italic(
                " " + syntax().ms1 +
                "+" + syntax().ms2 +
                "+" + syntax().ms3 +
                " ms")));
            log += syntax().log1;
            log += syntax().log2;
            log += syntax().log3;
            log.trace("");

            visited += path;
            for (auto [name, path]:
            syntax().dependencies.dependees) if (auto
            it  = doc::text::repo::map.find(path);
            it != doc::text::repo::map.end()
            and not visited.contains(
            it->second.model->path) and
            it->second.model->ready()) log.trace(
            it->second.model->log(visited)());
            return log;
        }

        report log () override  // don't ask until it's ready
        {
            array<std::filesystem::path> visited;
            return log(visited);
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