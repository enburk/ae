#pragma once
#include <filesystem>
#include "doc_ae_syntax_scopes.h"
#include "doc_ae_synthesis.h"
namespace doc::ae::translator
{
    syntax::scope global_scope;

    void add_library (std::filesystem::path path)
    {
    }

    /*
    void print (const entity & entity, array<str> & lines, int indent = 0, bool semicolon = true)
    {
        str line;


    
        for (Token token : entity.tokens) line += token.print () + " "; line.truncate ();

        if (line != "" && !line.headed ("#") && entity.body.size () == 0 && semicolon) line += ";"; line += entity.debug;

        if (line != "" && !line.headed ("#")) line = string (' ', indent) + line;

        if (line != "") lines += line;

        if (entity.body.size () > 0)
        {
            bool inner_semicolon = entity.kind != "enum" && entity.kind != "declarator";
            bool outer_semicolon = entity.kind == "enum" || entity.kind == "declarator"
                || entity.kind == "function"
                || entity.kind == "lambda" 
                || entity.kind == "class"
                || entity.kind == "union";
            
            lines += string (' ', indent) + "{"; for (auto & e : entity.body) print (e, lines, indent + 4, inner_semicolon);
            lines += string (' ', indent) + "}"; if (outer_semicolon) lines.back () += ";";
        }
    }

    string translator (string filename, std::string & result)
    {
        array<Source> sources; auto tokens = preprocess (sources, filename); try
        {
            array<Entity> entities = syntax::parsing (tokens);

            syntax::Scope scope; scope.pass (entities);

            synthesis (entities);

            array<string> lines; for (auto & entity : entities) print (entity, lines);

            result += "\xEF" "\xBB" "\xBF"; // UTF-8 BOM

            for (const auto & line : lines) result += line + "\n";
        }
        catch (Error e)
        {
            Source & source = sources [e.token.source];
            string under = string (' ', e.token.column); under += '^';
            string where = std::to_string (e.token.line+1) + ":" + std::to_string (e.token.column+1);
            string total = source.filename + " (" + where + "):\n\n" + e.what + "\n\n" + source.lines [e.token.line] + "\n" + under + "\n\n`" + e.token.text + "`\n";
            return total;
        }
        return "";
    }
    */
}

