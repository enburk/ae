#pragma once
#include "doc_text_model_b.h"
#include "doc_ae_lexica.h"
//#include "doc_ae_syntax.h"
namespace doc::ae
{
    using doc::text::report;

    struct model : doc::text::model
    {
        using base = doc::text::model;

        virtual void tokenize ()
        {
            tokens = lexica::parse(*this);

            report log;

            //syntax::parser(log).proceed(tokens);
        }
    };
}