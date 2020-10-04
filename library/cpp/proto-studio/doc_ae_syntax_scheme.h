#pragma once
#include "doc_ae_syntax_schema.h"
namespace doc::ae::syntax
{
    inline statement statementor::read_statement ()
    {
        str schema, source; array<token*> names;

        for (auto & e : elements)
        {
            schema += " ";
            source += " " + (
                e.kind == "()" or
                e.kind == "{}" ?
                e.name : e.opening->text);

            keywording(e);

            if (e.name == "{}") schema += "{}"; else
            if (e.kind == "()") schema += "()"; else
            if (e.opening->text == "=" ) schema += "=";  else
            if (e.opening->text == "," ) schema += ",";  else
            if (e.opening->text == ":" ) schema += ":";  else
            if (e.opening->text == "::") schema += "::"; else
            if (e.opening->text == ":=") schema += ":="; else
            if (e.opening->text == (char*)(u8"←")) schema += "<-"; else
            if (e.opening->text == (char*)(u8"→")) schema += "->"; else
            if (e.opening->kind == "keyword")
                schema += e.opening->text; else
                schema += e.opening->kind;
            
            if (schema.ends_with(" name")) names += e.opening;
            if (schema.ends_with(" names , name")) schema.truncate(schema.size() - 7);
            if (schema.ends_with(" name , name")){ schema.truncate(schema.size() - 7);
                schema += "s";
            }
        }
        if (schema != "") schema.erase(0); // leading " "
        if (source != "") source.erase(0); // leading " "

        auto schema_starts_with = [&schema](str s) { return
                schema.starts_with(s) and (
                schema.size() == s.size() or
                schema[s.size()] == ' ');
        };

        if (schema_starts_with("using"))
        {
            pragma s;
            s.title = read_token();
            s.arg = read_expression(); if (not elements.empty())
            s.body = read_statement_or_body();
            return statement{std::move(s)};
        }

        if (schema_starts_with("return"))
        {
            directive s;
            s.title = read_token(); if (not elements.empty())
            s.arg = read_expression();
            return statement{std::move(s)};
        }
        if (schema_starts_with("break")
        or  schema_starts_with("continue"))
        {
            directive s;
            s.title = read_token();
            return statement{std::move(s)};
        }
        if (schema_starts_with("await")
        or  schema_starts_with("yield")
        or  schema_starts_with("goto"))
        {
            directive s;
            s.title = read_token();
            s.arg = read_expression();
            return statement{std::move(s)};
        }

        if (schema_starts_with("if"))
        {
            conditional s;
            s.title = read("if");
            s.condition = read_expression_until("then");
            s.then_body = read_statement_or_body(); // if (not elements.empty()) { read("else");
         // s.else_body = read_statement_or_body(); }
            return statement{std::move(s)};
        }
        if (schema_starts_with("else"))
        {
            conditional s;
            s.title = read_token();
            s.else_body = read_statement_or_body();
            return statement{std::move(s)};
        }
        if (schema_starts_with("for"))
        {
            loop_for s;
            s.title = read_token();
            read("each"); s.names = read_list_of_names();
            read("in"); s.range = read_named_pack();
            read("do"); s.body = read_statement_or_body();
            return statement{std::move(s)};
        }
        if (schema_starts_with("while"))
        {
            loop_while s;
            s.title = read_token();
            s.condition = read_expression_until("do");
            s.body = read_statement_or_body();
            return statement{std::move(s)};
        }
        if (schema_starts_with("until"))
        {
            loop_while s;
            s.title = read_token();
            s.condition = read_expression_until("do");
            s.body = read_statement_or_body();
            return statement{std::move(s)};
        }

        if (schema == "name {}")
        {
            definition s;
            s.kind = "singleton";
            s.name = read_name();
            s.body = read_statement_or_body();
            return statement{std::move(s)};
        }

        if (schema_starts_with("name ="))
        {
            declaration s;
            s.kind = "constant";
            s.names = read_list_of_names();
            s.body = read_statement_or_body();
            return statement{std::move(s)};
        }

        if (schema_starts_with("type name ="))
        {
            definition s;
            s.kind = "type"; read("type");
            s.name = read_name(); read("=");
            s.body = read_statement_or_body();
            return statement{std::move(s), schema, source};
        }
        if (schema_starts_with("type name :"))
        {
            definition s;
            s.kind = "type"; read("type");
            s.name = read_name(); read(":");
            s.classes = read_list_of_named_packs(); read("=");
            s.body = read_statement_or_body();
            return statement{std::move(s), schema, source};
        }
        if (schema_starts_with("type name () ="))
        {
            definition s;
            s.kind = "type"; read("type");
            s.name = read_name();
            s.parameters = read_parameters().list; read("=");
            s.body = read_statement_or_body();
            return statement{std::move(s), schema, source};
        }
        if (schema_starts_with("type name () :"))
        {
            definition s;
            s.kind = "type"; read("type");
            s.name = read_name();
            s.parameters = read_parameters().list; read(":");
            s.classes = read_list_of_named_packs(); read("=");
            s.body = read_statement_or_body();
            return statement{std::move(s), schema, source};
        }
        if (schema_starts_with("type name extension =")
        or  schema_starts_with("type type extension ="))
        {
            definition s;
            s.kind = "type extension"; read("type");
            s.name = read_name();
            read("extension"); read("=");
            s.body = read_statement_or_body();
            return statement{std::move(s), schema, source};
        }
        if (schema_starts_with("type name () extension ="))
        {
            definition s;
            s.kind = "type extension"; read("type");
            s.name = read_name();
            s.parameters = read_parameters().list; read("extension"); read("=");
            s.body = read_statement_or_body();
            return statement{std::move(s), schema, source};
        }
        if (schema_starts_with("name : type ="))
        {
            definition s;
            s.kind = "type";
            s.name = read_name();
            read(":"); read("type"); read("=");
            s.body = read_statement_or_body();
            return statement{std::move(s), schema, source};
        }
        if (schema_starts_with("name () : type ="))
        {
            definition s;
            s.kind = "type";
            s.name = read_name();
            s.parameters = read_parameters().list;
            read(":"); read("type"); read("=");
            s.body = read_statement_or_body();
            return statement{std::move(s), schema, source};
        }

        if (schema_starts_with("name :="))
        {
            declaration s;
            s.kind = "variable";
            s.names = read_list_of_names(); read(":=");
            s.body = read_statement_or_body();
            return statement{std::move(s), schema, source};
        }

        if (schema_starts_with("name :")
        or  schema_starts_with("names :"))
        {
            declaration s;
            s.kind = "variable";
            s.names = read_list_of_names(); read(":");
            s.type = read_named_pack();
            if (not elements.empty()) { read("=");
                s.body = read_statement_or_body();
            }
            return statement{std::move(s), schema, source};
        }

        if (schema_starts_with("function name =")
        or  schema_starts_with("function name ->"))
        {
            subroutine s;
            s.title = read_token();
            s.kind = s.title->text;
            s.name = read_name();
            s.type = read_optional_type(); read("=");
            s.body = read_statement_or_body();
            return statement{std::move(s), schema, source};
        }
        if (schema_starts_with("mutation name <-"))
        {
            subroutine s;
            s.title = read_token();
            s.kind = s.title->text;
            s.name = read_name(); read((char*)(u8"←"));
            s.parameters.list += parameter{s.name, read_named_pack()}; read("=");
            s.body = read_statement_or_body();
            return statement{std::move(s), schema, source};
        }
        if (schema_starts_with("function name")
        or  schema_starts_with("mutation name"))
        {
            subroutine s;
            s.title = read_token();
            s.kind = s.title->text;
            s.name = read_name();
            s.parameters = read_parameters();
            s.type = read_optional_type(); read("=");
            s.body = read_statement_or_body();
            return statement{std::move(s), schema, source};
        }
        if (schema_starts_with("constructor"))
        {
            subroutine s;
            s.title = read_token();
            s.kind = s.title->text;
            s.parameters = read_parameters(); read("=");
            s.body = read_statement_or_body();
            return statement{std::move(s), schema, source};
        }
        if (schema_starts_with("destructor"))
        {
            subroutine s;
            s.title = read_token();
            s.kind = s.title->text; read("=");
            s.body = read_statement_or_body();
            return statement{std::move(s), schema, source};
        }

        if (schema_starts_with("operator"))
        {
            schema.replace_all(" name "  , " x ");
            schema.replace_all(" symbol ", " x ");

            subroutine s; s.title = read("operator");

            if (schema_starts_with("operator () x ()"))
            {
                s.kind = "binary operator";
                s.parameters = read_one_parameter();
                s.name = read_token();
                s.parameters.list += read_one_parameter().list;
                s.type = read_optional_type(); read("=");
                s.body = read_statement_or_body();
                return statement{std::move(s)};
            }
            if (schema_starts_with("operator () x"))
            {
                s.kind = "postfix operator";
                s.parameters = read_one_parameter();
                s.name = read_token();
                s.type = read_optional_type(); read("=");
                s.body = read_statement_or_body();
                return statement{std::move(s)};
            }
            if (schema_starts_with("operator ()"))
            {
                s.kind = "call operator";
                s.parameters = read_parameters();
                s.type = read_optional_type(); read("=");
                s.body = read_statement_or_body();
                return statement{std::move(s)};
            }
            if (schema_starts_with("operator x () x"))
            {
                s.kind = "outfix operator";
                s.name = read_token();
                s.parameters = read_one_parameter();
                s.coname = read_token();
                s.type = read_optional_type(); read("=");
                s.body = read_statement_or_body();
                return statement{std::move(s)};
            }
            if (schema_starts_with("operator x ()"))
            {
                s.kind = "prefix operator";
                s.name = read_token();
                s.parameters = read_one_parameter();
                s.type = read_optional_type(); read("=");
                s.body = read_statement_or_body();
                return statement{std::move(s)};
            }
            if (schema_starts_with("operator x"))
            {
                s.kind = "operator";
                s.name = read_token();
                s.type = read_optional_type(); read("=");
                s.body = read_statement_or_body();
                return statement{std::move(s)};
            }

            if (elements.size() == 0)
            throw error ("expected parameter, name or symbol");
            throw error ("expected '='");
        }

        return statement{read_expression(), schema, source};
    }
}

