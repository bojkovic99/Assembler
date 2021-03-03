#ifndef RELOKACIONA_H
#define RELOKACIONA_H
#include <string>
#include <iostream>
#include <vector>
#include "TabelaSimbola.h"

using namespace std;

enum RelType
{
    R_386_16,
    R_386_PC16
};
class Relokacija
{
public:
    Relokacija(short offset, RelType relT, Simbol &s, string sekcija);

    void stampaj(ofstream &out);
    string dohvatiHexOffset() const;
    string dohvatiRelType() const;

    Simbol s;
    short offset;
    RelType relT;
    string sekcija;
};
class Relokaciona
{
public:
    Relokaciona(string sekcija);
    vector<Relokacija> &dohvatiRelokacije() { return relokacije; }

    void stampaj(ofstream &out);

    void obrisiRelokaciju(int broj, int ts, string sek);

    string sekcija;
    void dodajRelokaciju(short offset, RelType relT, Simbol &s, string sekcija);
    vector<Relokacija> relokacije;
};

#endif