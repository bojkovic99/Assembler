#ifndef TABELASIMBOLA_H
#define TABELASIMBOLA_H
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <list>
#include <iterator>

class Podaci;
class Obracanja;
class Relokaciona;

using namespace std;

class Pomocna
{
public:
    int pozicije;
    char znaci;
    bool velicine;
    bool skok;
    Pomocna(int pozicije, char znaci, bool velicine, bool skok)
    {
        this->pozicije = pozicije;
        this->znaci = znaci;
        this->velicine = velicine;
        this->skok = skok;
    }
};

class Simbol
{
public:
    Simbol(string name, string sekcija, bool sek, int offset, bool lokalna, bool ex = false);
    string name;
    string sekcija;
    bool lokalna;
    int offset;
    int size;
    int number;
    int flag;
    bool defined;
    bool isEqu;
    bool sek;
    bool ex;
    vector<Pomocna> pomocna;

    void stampaj(ofstream &out);
};

class TabelaSimbola
{
public:
    map<string, Simbol> symbols;
    TabelaSimbola();
    bool dodajNoviSimbol(string token, string trenutnasekcija, int locationcounter, bool b2, bool ex = false);
    bool dodajNovuSekciju(string token, string trenutnasekcija, int locationcounter, string starasekcija, int starilc);
    void definisiSimbol(Simbol *simb, vector<Podaci *> sekcija, vector<Relokaciona *> rt, Obracanja *to);

    int dohvatiOffset(string name, string sekcija);

    Simbol *dohvatiSimbol(string naziv);
    void dodajPom(Simbol *sim, int posizcije, char znaci, bool velicine, bool skok = false);
    bool dalijesveDefinisano();

    void stampaj(ofstream &out);
    int counter;
    ~TabelaSimbola()
    {
    }

private:
};

#endif