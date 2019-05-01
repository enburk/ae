array<Entity> parsing (array<Token> & input)
{
    array<Entity> output; Entity entity; input += Token (";");
    
    for (auto token : input)
    {
        if (token.type == "{}") entity.body = parsing (token.tokens) + Entity () /* could be empty */; else

        if (token.text != ";" && !token.text.headed ("#")) { entity.tokens += token; continue; }
            
        if (entity.tokens.size () > 0 || entity.body.size () > 0) output += entity; entity = Entity ();

        if (token.text.headed ("#")) { entity.tokens += token; entity.kind = "pragma"; output += entity; entity = Entity (); }
    }

    return output;
}
array<Entity> parsing (deque<Token> & input) { return parsing (bracketing (input)); }
