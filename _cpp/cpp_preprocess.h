deque<Token> preprocess (array<Source> & sources, string filename)
{
    std::cout << filename << std::endl;

    std::ifstream stream (filename); std::string file {(
    std::istreambuf_iterator<char>(stream)),
    std::istreambuf_iterator<char>()};

    sources += Source (filename);
    
    deque<Token> output, line, input = lexica (sources, file);
    
    while (input.size () > 0)
    {
        auto token = input.front (); input.pop_front ();

        if (line.size () == 0 && token.type == "space") continue;

        if (line.size () == 1 && line.front ().text.headed ("#") && token.text != "\n") { line.front ().text += token.text; continue; }

        if (line.size () == 2 && line [0].text == "using" && line [1].type == "literal")
        {
            string filename = line [1].text; filename.strip ('\"');
                    
            string dir = sources.back ().filename; size_t n = dir.rfind ("/"); if (n != string::npos) filename = dir.head (n+1) + filename;
                    
            output += preprocess (sources, filename + ".dpp"); line.clear (); continue;
        }

        if (token.text == "\n") { output += line; line.clear (); continue; }

        if (token.type != "space") line += token;
    }

    return output;
}
