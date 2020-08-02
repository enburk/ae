#pragma once
#include <map>
#include "doc_ae_syntax.h"
namespace doc::ae::syntax
{
    struct scope
    {
        //struct signature { token* name; array<named_pack> args;
        //    bool operator != (const signature & s) const { return ! (*this == s); }
        //    bool operator == (const signature & s) const { return
        //        name == s.name &&
        //        args == s.args; }
        //};
        struct member { named_pack type; };
        using signature = str;

        scope * outer = nullptr;
        std::list<scope> unnamed;
        std::map<str, scope> named;
        std::map<signature, member> members;

        void add (const scope & scope) {
            for (auto [str, sco] : scope.named)
                named.emplace(str, sco);
            for (auto [sig, mem] : scope.members)
                members.emplace(sig, mem);
        }

        void fill (array<statement> & input, report & log)
        {
            for (auto & statement : input)
            {
                statement.scope = this;

                std::visit(aux::overloaded
                {
                    [this, &log](loop_for    s) { add(log, s); },
                    [this, &log](loop_while  s) { add(log, s); },
                    [this, &log](expression  s) { add(log, s); },
                    [this, &log](conditional s) { add(log, s); },
                    [this, &log](declaration s) { add(log, s); },
                    [this, &log](subroutine  s) { add(log, s); },
                    [this, &log](pragma      s) { add(log, s); },
                },
                statement.variant);
            }
        }

        void add (report & log, loop_for    & s) {  }
        void add (report & log, loop_while  & s) {  }
        void add (report & log, expression  & s) {  }
        void add (report & log, conditional & s) {  }
        void add (report & log, declaration & s)
        {
            for (auto name : s.names)
            {
                auto [it, inserted] = members.emplace(name->text, member{s.type});
                if (!inserted) { log.error(name, "name '" + name->text
                    + "' already exists"); return; }

                if (s.kind == "singleton" || s.kind == "class" || s.kind == "union")
                    named.emplace(name->text, scope{.outer=this})
                        .first->second.fill(s.body, log);
            }
        }
        void add (report & log, subroutine  & s)
        {
            signature signature = s.name->text;
            signature += "("; //for (auto & p : s.parameters)
            //signature += type(p.type);
            signature += ")";

            auto [it, inserted] = members.emplace(signature, member{s.type});
            if (!inserted) { log.error(s.name, "signature '"
                + html::lexica::encoded(signature)
                + "' already exists"); return; }

            unnamed.emplace_back(scope{.outer=this}).fill(s.body, log);
        }
        void add (report & log, pragma      & s) {  }




        /*
        Scope * find (string & name) 
        {
            Scope * scope = this;

            if (name == "::") { while (scope->outer != nullptr) scope = scope->outer; return scope; }

            auto it1 = typers.find (name); if (it1 != typers.end ()) return find (it1->second); name += " ()";
            auto it2 = typers.find (name); if (it2 != typers.end ()) return find (it2->second); name.truncate (); name.truncate (); name.truncate ();
            auto it3 = scopes.find (name); if (it3 != scopes.end ()) return &    (it3->second);

            if (outer != nullptr) return outer->find (name);

            return nullptr;
        }

        Scope * find (Token & token) 
        {
            Scope * scope = this;

            Name combiname (token); for (auto & name : combiname.names)
            {
                for (auto & t : name.tokens)
                {
                    if (t.type == "()") { scope = nullptr; break; } // too complicated
                    if (t.type == "[]") { scope = nullptr; break; } // too complicated
                    if (t.type == "->") { scope = nullptr; break; } // too complicated
                }

                if (scope == nullptr) break; scope = scope->find (name.text);
                if (scope == nullptr) break;
            }
            token = combiname.combine ();

            return scope;
        }

        void pass (Token & token) 
        {
            if (token.type != "name") { for (auto & t : token.tokens) pass (t); return; }

            for (auto & t : token.tokens) for (auto & tt : t.tokens) pass (tt);

            find (token);
        }

        void pass (array<Entity> & entities)
        {
            for (auto & entity : entities)
            {
                schema (entity);
        
                if (entity.kind == "variable" ) typers [entity.name.text] = entity.type; else
                if (entity.kind == "function" )
                {
                    string s = entity.name.text + " ("; for (auto token : entity.name.tokens) if (token.type == "()") for (auto t : token.tokens)
                    {
                        if (t.text == ",") s += ", "; else if (t.type == "name") if (s.tailed ("(") || s.tailed (" ")) s += t.text;
                    }
                    s += ")"; typers [s] = entity.type;
                }
            }

            for (auto & entity : entities)
            {
                pass (entity.name); pass (entity.type); for (auto & token : entity.tokens) pass (token);

                if (entity.body.size () > 0)
                {
                    string name = entity.kind == "namespace" || entity.kind == "class" ? entity.name.text : "";

                    Scope & scope = scopes [name]; scope.outer = this;

                    if (entity.kind == "function" )
                    {
                        array<Token> names; for (auto token : entity.name.tokens) if (token.type == "()") for (auto t : token.tokens)
                        {
                            if (t.text == ",") names.clear (); else if (t.type == "name") names += t;
                        
                            if (names.size () == 2) scope.typers [names [1].text] = names [0];
                        }
                    }

                    scope.pass (entity.body); if (name == "") scope = Scope ();
                }
            }

            if (1) for (auto typer : typers) { Entity e; e.debug = "// in scope: " + typer.first + u8" -> " + typer.second.print (); entities = e + entities; }
        }
        */
    };



}

        // Named:
        // 1. type
        // 2. singleton (type + variable)
        // Unnamed:
        // 1. function/operator
        // 2. block "if", "else", "for", "do", "while", "until"
        // 3. const/variable initializer list ?

        // Scopes population:
        // 1. const/variable
        // 2. function/operator args
        // 3. block variables in "if", "for", "catch"
        // 4. import ?

