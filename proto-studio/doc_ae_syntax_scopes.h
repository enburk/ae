#pragma once
#include <map>
#include <list>
#include "doc_ae_syntax.h"
namespace doc::ae::syntax
{
    struct scope
    {
        scope* outer = nullptr;
        std::list<scope> unnamed;
        std::map<str, scope> named;
        std::map<str, scope*> outscopes;

        struct member
        {
            str name, kind;
            parameters args;
            namepack type;
        };
        std::multimap
        <str, member>
            members;

        scope () = default;
        scope
        (
            report& log,
            array<statement>& body,
            parameters args = {},
            scope* outer = nullptr
        ) : outer (outer)
        {
            for (auto& arg: args.list) if (arg.name)
            {
                if (members.contains(arg.name->text))
                    log.error(arg.name, "parameter " +
                        arg.name->text +
                        " already exists");
                else
                members.emplace(
                arg.name->text, member{
                arg.name->text, "variable", {},
                arg.type});
            }

            for (auto& statement: body)
            {
                statement.scope = this;

                if (statement.name) members.emplace(
                    statement.name->text, member{
                    statement.name->text,
                    statement.kind,
                    statement.args,
                    statement.type});

                if (statement.name and
                   (statement.kind == "type" or
                    statement.kind == "singleton"))
                {
                    if (members.contains(statement.name->text))
                        log.error(statement.name, "name " +
                            statement.name->text +
                            " already defined");
                    else
                    named.emplace(
                    statement.name->text, scope{log,
                    statement.body,
                    statement.args,
                    this});
                }
                else
                {
                    unnamed.emplace_back( scope{log,
                    statement.body,
                    statement.args,
                    this});
                }
            }
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

