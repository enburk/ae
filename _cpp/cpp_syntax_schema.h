void schema (Entity & entity)
{
    entity.tokens = combinaming (entity.tokens);
        
    static std::set<string> ignored = {"inline", "noexcept", "const", "static", "mutating", "friend", "virtual", "explicit", "public", "*", "&", "&&"};

    array<Token> tt; for (auto t : entity.tokens) if (ignored.find (t.text) == ignored.end ()) tt += t;
        
    string ss; for (auto t : tt) ss += (t.type == "keyword" || t.type == "symbol" ? t.text : t.type) + " "; ss.truncate ();
        
    bool bodied = entity.body.size () > 0; bool bodiless = !bodied;

    if (ss ==     "union"                    ){ entity.kind = "union";                           } else
    if (ss ==     "union name"               ){ entity.kind = "union";     entity.name = tt [1]; } else
    if (ss == "namespace name"               ){ entity.kind = "namespace"; entity.name = tt [1]; } else
    if (ss ==      "enum name"               ){ entity.kind = "enum";      entity.name = tt [1]; } else
    if (ss =="enum class name"               ){ entity.kind = "enum";      entity.name = tt [2]; } else
    if (ss ==     "class name"               ){ entity.kind = "class";     entity.name = tt [1]; } else
    if (ss ==    "struct name"               ){ entity.kind = "class";     entity.name = tt [1]; } else
    if (ss ==     "class name : name"        ){ entity.kind = "class";     entity.name = tt [1]; entity.type = tt [3]; } else
    if (ss ==    "struct name : name"        ){ entity.kind = "class";     entity.name = tt [1]; entity.type = tt [3]; } else
    if (ss ==           "name ==" && bodied  ){ entity.kind = "lambda";    entity.name = tt [0]; entity.type = "auto"; } else
    if (ss ==  u8"name → name ==" && bodied  ){ entity.kind = "lambda";    entity.name = tt [0]; entity.type = tt [2]; } else
    if (ss ==           "name ="  && bodied  ){ entity.kind = "function";  entity.name = tt [0]; entity.type = "auto"; } else
    if (ss ==  u8"name → name ="  && bodied  ){ entity.kind = "function";  entity.name = tt [0]; entity.type = tt [2]; } else
    if (ss ==      "name name"    && bodiless){ entity.kind = "variable";  entity.name = tt [1]; entity.type = tt [0]; } else
    if (ss.headed ("name name =")            ){ entity.kind = "variable";  entity.name = tt [1]; entity.type = tt [0]; } else
    {}

    if (entity.kind == "class" && entity.type.text != "")
    {
        Entity baseclass; baseclass.tokens += Token ("using baseclass = " + entity.type.print ()); entity.body = baseclass + entity.body;
    }

    if (1) if (entity.tokens.size () > 0) entity.debug += " // " + entity.kind;
    if (1) if (entity.tokens.size () > 0) entity.debug += " // " + ss;
}
