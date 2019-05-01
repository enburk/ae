array<Token> combinaming (array<Token> input)
{
    array<Token> output; for (auto t : input)
    {
        t.tokens = combinaming (t.tokens);

        if (output.size () > 0)
        {
            auto & last = output.back ();

            if (last.type == "name::" && t.type == "name") { last.tokens += t; last.type = "name";   continue; }
            if (last.type == "name"   && t.text ==   "->") { last.tokens += t; last.type = "name::"; continue; }
            if (last.type == "name"   && t.text ==   "::") { last.tokens += t; last.type = "name::"; continue; }
            if (last.type == "name"   && t.text ==    ".") { last.tokens += t; last.type = "name::"; continue; }
            if (last.type == "name"   && t.type == u8"«»") { last.tokens += t; continue; }
            if (last.type == "name"   && t.type ==   "()") { last.tokens += t; continue; }
            if (last.type == "name"   && t.type ==   "[]") { last.tokens += t; continue; }
        }

        static std::set<string> keywords =
        {
            "struct", "class", "enum", "union", "namespace", "operator", "using",

            "inline", "noexcept", "const", "mutating", "static", "friend", "virtual", "explicit",

            "if", "then", "else", "for", "while", "do", "until", "return", "break", "continue"
        };

        if (keywords.find (t.text) != keywords.end ()) t.type = "keyword";

        if (t.text == "::") t.type = "name";

        output += t;
    }

    return output;
}

struct Name
{
    array<Token> names;

    Name (Token token) { names += token; names [0].tokens.clear (); for (auto t : token.tokens) if (t.type == "name") names += t; else names.back ().tokens += t; }

    Token combine () const { Token token = names [0]; for (int i=1; i<names.size (); i++) token.tokens += names [i]; return token; }
};
