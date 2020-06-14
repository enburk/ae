#pragma once
#include "doc_ae_syntax_parser.h"
//#include "doc_ae_syntax_schema.h"
#include "doc_ae_syntax_scopes.h"
namespace doc::ae::syntax
{
    struct analysis
    {
        scope global_scope;

        report & log;

        analysis (report & log) : log(log) {}

        void proceed (array<token> & tokens)
        {
            parser(log).proceed(tokens);
        }
    };
}

