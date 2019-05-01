void synthesis (array<Entity> & entities, string scope_kind = "")
{
    for (auto & entity : entities)
    {
        if (entity.kind == "class")
        {
            entity.tokens.clear ();                               if (entity.name.tokens.size  () > 0)
            entity.tokens += Token ("template") + entity.name.tokens; entity.name.tokens.clear ();
            entity.tokens += Token ( "struct" ) + entity.name;    if (entity.type.text != "")
            entity.tokens += Token (": public") + entity.type;
        }
        else
        if (entity.kind == "function" && scope_kind == "function" || entity.kind == "lambda")
        {
            entity.tokens.clear ();
            entity.tokens += entity.type;
            entity.tokens += Token (entity.name.text);
            entity.tokens += Token ( "=" );
            entity.tokens += Token ("[=]");
            entity.tokens.back ().tokens += entity.name.tokens;
        }
        else
        if (entity.kind == "function")
        {
            bool mutating = 
            entity.tokens [0].text == "mutating";
            entity.tokens.clear ();
            entity.tokens += entity.type;
            entity.tokens += entity.name; if (!mutating && scope_kind == "class")
            entity.tokens += Token ("const");
        }

        synthesis (entity.body, entity.kind); 
    }
}

