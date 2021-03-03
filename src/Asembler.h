#ifndef asembler_h
#define asembler_h
#include <vector>
#include <string>
#include <regex>
#include <iostream>
#include <fstream>

using namespace std;

class TabelaSimbola;
class Podaci;
class Obracanja;
class Relokaciona;

class Asembler
{
public:
    Asembler(ifstream &in, ofstream &out);
    void generisiTokene();

    ifstream *in;
    ofstream *out;
    int locationcounter = 0;
    int poslinstrukcija = 0;
    TabelaSimbola *ts;
    Obracanja *to;
    static string tekucaSekcija;
    vector<Relokaciona *> relokacione;
    vector<Podaci *> podaci;
    Relokaciona *rt;
    Podaci *s;

    bool obradiTokene(vector<string> tokeni, string *poruka);

    void povecajLocationcounter(int broj);

    void generisiPodatke(string token);
    void generisiPodatkeW(string token);
    void generisiPodatkeSkok(string token);

    bool generisiKodSkok(string token);
    bool generisiKod(string token);

    bool dalijesveDefinisano();
};

#endif