#pragma once
#include <map>
#include "doc_ae_syntax_visitor.h"
#include "doc_ae_syntax_expressions.h"
namespace doc::ae::syntax::typing
{
    //auto apply (statement& scope, str name, brackets args)
    //{
    //    //statement* match = nullptr;
    //    //int matchingness = 0;
    //    //
    //    //for (auto
    //    //i  = scope.members.lower_bound(name);
    //    //i != scope.members.upper_bound(name); ++i)
    //    //{
    //    //    // if (i->args.size()
    //    //    // <=  j->second.args.size())
    //    //    // // ...
    //    //    //     match = 
    //    //}
    //
    //    //if (not match) { log.error(
    //    //    i->identifier,
    //    //    name + " not found");
    //    //    break; }
    //    //
    //    //if (match->type.name == "") { log.error(
    //    //    match->name,
    //    //    match->name->text +
    //    //    " wasn't yet evaluated");
    //    //    break; }
    //}
    //
    //auto apply (statement& scope, array<brackets> argss)
    //{
    //    //if (argss.empty()) return scope.type;
    //    //brackets args = argss.front();
    //    //argss.erase(0);
    //    //
    //    //if (scope.kind == "type")
    //    //{
    //    //    str name;
    //    //    if (args.opening) name = args.opening->text;
    //    //    if (args.closing) name = args.closing->text;
    //    //
    //    //}
    //}

    void parse (statement& module, report& log, auto& cancel)
    {
        visitor visitor;
        visitor.on_statement = [&](statement& s)
        {
            if (s.kind == "singleton")
            {
                s.type.name =
                s.names.front()->text;
            }
            if (s.kind == "variable"
            or  s.kind == "constant")
            {
                s.type.name =
                s.typexpr.type.name != "" ?
                s.typexpr.type.name :
                s.   expr.type.name;
            }
            if (s.kind == "function")
            {
                for (auto& arg: s.args.list)
                s.type.name += arg.type.name + (char*)(u8"→");
                s.type.name += s.typexpr.type.name;
            }
            if (s.kind == "type")
            {
                for (auto& arg: s.args.list)
                s.type.name += arg.type.name + (char*)(u8"→");
                s.type.name += s.names.front()->text;
            }
        };
        visitor.on_namepack = [&](namepack& pack, statement* scope)
        {
            auto i = pack.names.begin(); if (i->coloncolon)
            {
                pack.type.kind = "module";
                pack.type.name = "::" +
                i->identifier->text;

                while  (scope->outer)
                scope = scope->outer;
                scope = scope->modules[
                i->identifier->text];

                //apply(scope, i->argss);
                i++;
            }
            for (; i != pack.names.end(); i++)
            {
                auto& unit = *i;
                str name = unit.identifier->text;

                if (not scope) {
                    log.error(
                    i->identifier, "'" +
                    name + "' out of scope");
                    break; }

                //auto args = unit.argss.begin();
                //
                //statement* match = nullptr;
                //int particularity = 0;
                //
                //for (auto
                //j  = scope->members.lower_bound(name);
                //j != scope->members.upper_bound(name); ++j)
                //{
                //    // if (i->args.size()
                //    // <=  j->second.args.size())
                //    // // ...
                //    //     match = 
                //}
                //
                //if (not match) { log.error(
                //    i->identifier,
                //    name + " not found");
                //    break; }

                //if (match->type.name == "") { log.error(
                //    match->name,
                //    match->name->text +
                //    " wasn't yet evaluated");
                //    break; }
                //
                //if (match->kind == "type")
                //{
                //    if (
                //    pack.type.name != "")
                //    pack.type.name += "::";
                //    pack.type.name += match->type.name;
                //    pack.type.kind  = match->type.kind;
                //    scope = match;
                //}
                //else
                //if (match->kind == "function")
                //{
                //}
                //else { log.error(
                //    match->name,
                //    match->name->text +
                //    " of unexpected kind " +
                //    match->kind);
                //    break; }
            }
        };
        visitor.on_expression = [&](expression& e, statement* scope)
        {
            expressions::parse(e, scope, log);
        };
        visitor.pass(module, cancel);

    }
}


