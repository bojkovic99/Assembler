#ifndef PODACI_H
#define PODACI_H
#include <string>
#include <iostream>
#include <map>
#include <stdint.h>
#include <vector>

using namespace std;
class Podaci
{
public:
    Podaci(int broj, string sekcija)
    {
        counter = broj;
        pocetni = broj;
        ime = sekcija;
    }
    int dodajPodatak(short val, int kol = 1);

    void promeniPodatak(int pozicija, short val, int kol = 1);
    unsigned char uzmiPodatak(int pozicija)
    {

        return podaci.at(pozicija);
    }

    string ime;

    string sl;

    int counter;
    int pocetni;
    map<int, unsigned char> podaci;

    void stampaj(ofstream &out);
};

#endif