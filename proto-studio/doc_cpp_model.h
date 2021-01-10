#pragma once
#include "doc_text_model_b.h"
#include "doc_cpp_lexica.h"
#include "doc_cpp_syntax.h"
namespace doc::cpp
{
    using doc::text::report;

    struct model : doc::text::model
    {
        using base = doc::text::model;

        virtual void tokenize ()
        {
            tokens = lexica::parse(*this);

            report log;

            syntax::parser(log).proceed(tokens);
        }
    };
}