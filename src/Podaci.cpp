#include "Podaci.h"
#include <fstream>

void Podaci::promeniPodatak(int pozicija, short val, int kol)
{
    if (pozicija >= counter || pozicija < pocetni)
        return;
    if (kol == 1)
    {
        podaci[pozicija] += val;
    }
    else
    {
        unsigned short stvarnavrednost = 0;
        unsigned char pom1 = uzmiPodatak(pozicija + 1);
        unsigned char pom2 = uzmiPodatak(pozicija);
        stvarnavrednost += pom1;
        stvarnavrednost = stvarnavrednost << 8;
        stvarnavrednost += pom2;
        short vr = stvarnavrednost;
        vr += val;

        char MASK = 0xFF;
        unsigned char value = (unsigned char)(vr & MASK);
        podaci[pozicija] = value;
        value = (vr >> 8) & MASK;
        podaci[pozicija + 1] = value;
    }
}
int Podaci::dodajPodatak(short val, int kol)
{

    if (kol == 1)
    {
        podaci.insert(std::pair<int, char>(counter, val));
        counter++;
        return counter - 1;
    }
    else
    {
        char MASK = 0xFF;
        int pom = counter;
        unsigned char value = (unsigned char)val & MASK;
        dodajPodatak(value);
        value = (val >> 8) & MASK;
        dodajPodatak(value);
        return pom;
    }
}

void Podaci::stampaj(ofstream &outt)
{
    outt << "#" << ime << endl;
    char str[10];
    for (auto it : podaci)
    {

        sprintf(str, "%02X", it.second);
        string out(str);
        outt << out << "\t";
    }
}