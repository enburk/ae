#pragma once
#include "doc_ae_syntax_parser.h"
#include "doc_ae_syntax_schema.h"
#include "doc_ae_syntax_scopes.h"
namespace doc::ae::syntax
{
    struct analysis
    {
        report & log;

        scope global_scope;

        analysis (report & log) : log(log) {}

        array<statement> proceed (array<token> & tokens)
        {
            return
                statementor(log).proceed(
                parser(log).proceed(
                tokens));
        }
    };
}

