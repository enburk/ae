#pragma once
#include <map>
#include "doc_ae_syntax.h"
namespace doc::ae::syntax
{
    struct scope
    {
        scope * outer = nullptr;
        std::list<scope> unnamed;
        std::map<str, scope> named;

        struct member
        {
            token* name;
            array<named_pack> args;
            named_pack type;
            int field = -1;
        };
        std::multimap<str, member> members;

        void fill (array<statement> & input)
        {
            for (auto & statement : input)
            {
                statement.scope = this;

                std::visit(aux::overloaded
                {
                    [this](loop_for    s) { add(s); },
                    [this](loop_while  s) { add(s); },
                    [this](expression  s) {},
                    [this](conditional s) { add(s); },
                    [this](declaration s) { add(s); },
                    [this](subroutine  s) { add(s); },
                    [this](pragma      s) {},
                },
                statement.variant);
            }
        }

        void add (loop_for & s)
        {
            unnamed.emplace_back(scope{.outer=this});

            if (s.names.size() == 1)
                unnamed.back().members.emplace(s.names[0]->text, 
                    member{.name = s.names[0], .type = s.range});
            else
            {
                int i = 0; for (auto name : s.names)
                unnamed.back().members.emplace(name->text, 
                    member{.name = name, .type = s.range, .field = i++});
            }
            
            unnamed.back().fill(s.body);
        }
        void add (loop_while & s)
        {
            unnamed.emplace_back(scope{.outer=this});
            unnamed.back().fill(s.body);
        }
        void add (conditional & s)
        {
            unnamed.emplace_back(scope{.outer=this});
            unnamed.back().fill(s.then_body);
            unnamed.emplace_back(scope{.outer=this});
            unnamed.back().fill(s.else_body);
        }
        void add (subroutine & s)
        {
            member m {.name = s.name, .type = s.type};
            for (auto & p : s.parameters) m.args += p.type;
            members.emplace(m.name->text, m);
            unnamed.emplace_back(scope{.outer=this})
                .fill(s.body);
        }
        void add (declaration & s)
        {
            for (auto name : s.names) {
                members.emplace(name->text, member{.name = name, .type = s.type});
                if (s.kind == "singleton" || s.kind == "class" || s.kind == "union")
                    named.emplace(name->text, scope{.outer=this})
                        .first->second.fill(s.body);
            }
        }

        void check ()
        {
                //auto [it, inserted] = members.emplace(name->text, member{s.type});
                //if (!inserted) { log.error(name, "name '" + name->text
                //    + "' already exists"); return; }
            // auto [it, inserted] = members.emplace(signature, member{s.type});
            // if (!inserted) { log.error(s.name, "signature '"
            //     + html::lexica::encoded(signature)
            //     + "' already exists"); return; }
        }


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

