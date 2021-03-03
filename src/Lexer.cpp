#include "Lexer.h"

vector<string> Lexer::napraviTokene(string linija)
{
    vector<string> niz;
    while (!linija.empty())
    {
        if (Lexer::enter(linija.at(0)))
        {
            linija.resize(0);
            break;
        }

        int i = 0;
        while (i < linija.length() && (Lexer::tab(linija.at(i)) || Lexer::spejs(linija.at(i)) || Lexer::zarez(linija.at(i)) || Lexer::plus(linija.at(i)) || Lexer::minus(linija.at(i))))
        {
            if (Lexer::zarez(linija.at(i)) || Lexer::plus(linija.at(i)) || Lexer::minus(linija.at(i)))
            {
                string pom = "";
                pom += linija.at(i);
                niz.push_back(pom);
            }
            i++;
        }
        linija.erase(0, i);

        int brojac = 0;
        while (linija.size() != 0 && brojac != linija.size() && !(Lexer::tab(linija.at(brojac)) || Lexer::spejs(linija.at(brojac)) || Lexer::zarez(linija.at(brojac)) || Lexer::plus(linija.at(brojac)) || Lexer::minus(linija.at(brojac)) || Lexer::enter(linija.at(brojac))))
        {
            brojac++;
        }
        if (brojac > 0)
        {
            niz.push_back(linija.substr(0, brojac));
            linija.erase(0, brojac);
        }
        else
            break;
    }

    return niz;
}

bool Lexer::enter(char znak)
{
    return znak == '\n';
}
bool Lexer::tab(char znak)
{
    return znak == '\t';
}
bool Lexer::zarez(char znak)
{
    return znak == ',';
}
bool Lexer::plus(char znak)
{
    return znak == '+';
}
bool Lexer::minus(char znak)
{
    return znak == '-';
}
bool Lexer::spejs(char znak)
{
    return znak == ' ';
}