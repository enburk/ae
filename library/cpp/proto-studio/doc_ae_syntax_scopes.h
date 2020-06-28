#pragma once
#include <map>
#include "doc_ae_syntax.h"
//#include "doc_ae_syntax_schema.h"
namespace doc::ae::syntax
{
    struct scope
    {
        scope * outer = nullptr;
        std::map<str, scope> named;
        std::list<scope> unnamed;

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

        void fill (array<element> & input)
        {
            //for (auto & s : input)
            //{
            //    s.scope = this; if (s.body.size() > 0)
            //    {
            //        if (s.elements.size() > 0 &&
            //            std::holds_alternative<name>(s.elements.front())) {
            //            auto names = std::get<name>(s.elements.front()).names;
            //            if (names.size() != 1) { error(names[0].identifier,
            //                "unexpected qualified name with body");
            //                continue;
            //            }
            //
            //            scope * current = this;
            //            if (names[0].coloncolon) 
            //                while (current->outer)
            //                    current = current->outer;
            //
            //            str id = names[0].identifier->text;
            //            if (current->named.find(id) != current->named.end()) {
            //                error(names[0].identifier, "named scope '"+id+"' already exists");
            //                continue;
            //            }
            //
            //            // "if", "else", "for", "for.each", "do", "while", "until"
            //
            //        }
            //        else
            //        {
            //            unnamed.push_back(scope{.outer=this});
            //            unnamed.back().parse(s.body);
            //        }
            //    }
            //}
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

