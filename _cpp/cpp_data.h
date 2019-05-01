struct Token
{
    int source, line, column; string type, text, tail; array<Token> tokens;

    Token (string text = "") : source (0), line (0), column (0), text (text) {}

    Token & operator = (const Token  & s) = default;
    Token & operator = (const string & s) { *this = Token (s); return *this; }

    friend array<Token> operator + (const Token  & a, const Token  & b) { array<Token> tt; tt += a; tt+= b; return tt; }

    string print () const
    {
        string a = text; if (a == u8"«") a = "<"; if (type == "literal") a = "u8" + a;
        string b = tail; if (b == u8"»") b = ">";
            
        string s = a; for (auto t : tokens) s += " " + t.print (); if (b != "") s += " " + b;
        return s;
    }
};

struct Error  { string what; Token token; Error (string w, Token t) : what (w), token (t) {} };

struct Source { string filename; array<string> lines; Source (string fn) : filename (fn) {} };

struct Entity { string kind; Token name, type; array<Token> tokens; array<Entity> body; string debug; };



