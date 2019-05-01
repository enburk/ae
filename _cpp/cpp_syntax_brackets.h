array<Token> anglebracketing (array<Token> & input, bool inside = false)
{
    array<Token> output; while (input.size () > 0)
    {
        auto token = input.front (); input.erase (input.begin ());

        static std::set<string> fails = {"&&", "||", "?", ";", "{", "(", "[", u8"«"};

        if (inside) if (fails.find (token.text) != fails.end ())               { input.insert (input.begin (), token); break; }
        if (inside) if (token.text == ">>") { inside = false; token.text == ">"; input.insert (input.begin (), token); break; }
        if (inside) if (token.text == ">" ) { inside = false; break; }

        if (token.text == "<")
        {
            try { token.tokens = anglebracketing (input, true); } catch (Error) { output += token; continue; }

            token.type = u8"«»"; token.text = u8"«"; token.tail = u8"»";
        } 

        output += token;
    }

    if (inside) { input.insert (input.begin (), output.begin (), output.end ()); throw Error ("Angle brakets fail", input.front ()); }
    
    for (auto & token : output) token.tokens = anglebracketing (token.tokens);

    return output;
}

array<Token> bracketing (deque<Token> & input, string closing = "")
{
    array<Token> output; while (input.size () > 0)
    {
        auto token = input.front (); input.pop_front ();

        if (token.text == closing) break;
        if (token.text == u8"{") { token.type = u8"{}"; token.tail = u8"}"; token.tokens = bracketing (input, u8"}"); } else
        if (token.text == u8"(") { token.type = u8"()"; token.tail = u8")"; token.tokens = bracketing (input, u8")"); } else
        if (token.text == u8"[") { token.type = u8"[]"; token.tail = u8"]"; token.tokens = bracketing (input, u8"]"); } else
        if (token.text == u8"«") { token.type = u8"«»"; token.tail = u8"»"; token.tokens = bracketing (input, u8"»"); } 

        output += token;
    }
    return anglebracketing (output);
}
