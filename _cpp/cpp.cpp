#include <map>
#include <set>
#include <deque>
#include <vector>
#include <string>
#include <limits>
#include <memory>
#include <functional>
#include <algorithm>
#include <streambuf>
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>

namespace cpp
{
    #include "cpp_aux.h"
    #include "cpp_data.h"
    #include "cpp_lexica.h"
    #include "cpp_preprocess.h"
    namespace syntax
    {
        #include "cpp_syntax_brackets.h"
        #include "cpp_syntax_names.h"
        #include "cpp_syntax_parsing.h"
        #include "cpp_syntax_schema.h"
        #include "cpp_syntax_scopes.h"
    }
    #include "cpp_synthesis.h"
    #include "cpp_translator.h"
}

using namespace std;
using namespace std::filesystem;

int main ()
{
    cout << "ae.cpp.1.2 translator" << endl << endl;
    
    try { current_path ("../../_dpp"); } catch (...) { cout << "wrong dpp directory!" << endl << endl; return -1; }

    for (recursive_directory_iterator next (path (".")), end; next != end; ++next)
    {
        path fn = next->path (); if (is_regular_file (fn)) if (fn.extension () == ".dpp!")
        {
            string result, error = cpp::translator (fn.string (), result);
            
            if (error != ""){ cout << endl << "ERROR" << endl << error << endl; return -1; }

            fn.replace_extension (".cpp");

            ofstream (fn) << result;
        }
    }

    cout << endl;

    return 0;
}

/*

C++
template<class ForwardIt, class T>
void iota(ForwardIt first, ForwardIt last, T value)
{
    while(first != last) {
        *first++ = value;
        ++value;
    }
}
AE: only operator ++
b = ++a ~ ++(a); b = a;
b = a++ ~ b = a; ++(a);

*/