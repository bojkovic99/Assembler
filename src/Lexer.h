#ifndef lexer_h
#define lexer_h
#include <iostream>
#include <vector>

using namespace std;

class Lexer
{

public:
    static bool enter(char znak);
    static bool tab(char znak);
    static bool zarez(char znak);
    static vector<string> napraviTokene(string linija);
    static bool plus(char znak);
    static bool minus(char znak);
    static bool spejs(char znak);
};

#endif