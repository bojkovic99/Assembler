#ifndef OBRACANJA_H
#define OBRACANJA_H
#include <string>
#include <iostream>
#include <vector>
#include "TabelaSimbola.h"

using namespace std;

class TabelaSimbola;
class Podaci;
class Relokaciona;

class Obr
{
public:
    Simbol simboli;
    string znaci;
    Obr(Simbol sim, string znak) : simboli(sim)
    {
        this->znaci = znak;
    }
};

class Obracanje
{
public:
    short value = 0;
    struct Brojaci
    {
    public:
        short bss;
        short text;
        short data;
        short und;
    } brojaci;
    Obracanje();

    vector<Obr> sadrzaj;

    string naziv;
};
class Obracanja
{
public:
    string sekcija;
    TabelaSimbola *ts;
    vector<Obracanje> obracanja;

    Obracanja(string sekcija, TabelaSimbola *ts);

    void ubaciObracanje(string ime, string novobracanje, string znak, bool dalijebroj = false);

    void obrisiObracanja(string ime, vector<Podaci *> sekcije, vector<Relokaciona *> rt, Obracanja *to);
    void proveraObracanja(string proslediime, vector<Podaci *> sekcije, vector<Relokaciona *> rt, Obracanja *to);

    bool dalijesveDefinisano();
};

#endif