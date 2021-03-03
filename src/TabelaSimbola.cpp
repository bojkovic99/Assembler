#include "Podaci.h"
#include "Relokaciona.h"
#include "Obracanja.h"
#include "Asembler.h"
#include <map>
#include <fstream>
using namespace std;
TabelaSimbola::TabelaSimbola()
{
    counter = 0;

    Simbol *si = new Simbol(".UND", ".UND", true, 0, true);
    si->number = counter;
    counter++;
    symbols.insert(std::pair<string, Simbol>(".UND", *si));

    Simbol s = *dohvatiSimbol(".UND");
    auto it = symbols.find(s.name);
    if (it != symbols.end())
    {
        it->second.defined = true;
        it->second.sekcija = s.sekcija;
        it->second.offset = s.offset;
    }
}
bool TabelaSimbola::dodajNoviSimbol(string token, string tsekcija, int locationcounter, bool b2, bool b3)
{
    Simbol *s = new Simbol(token, tsekcija, false, locationcounter, b2, b3);
    if (symbols.find(s->name) == symbols.end())
    {
        s->number = counter;
        counter++;
        symbols.insert(std::pair<string, Simbol>(s->name, *s));
        return true;
    }
    else
    {
        return false;
    }
}
bool TabelaSimbola::dodajNovuSekciju(string token, string tsekcija, int locationcounter, string starasekcija, int starilc)
{
    auto it2 = symbols.find(starasekcija);
    if (it2 != symbols.end())
    {
        it2->second.size = starilc;
    }
    Simbol *s = new Simbol(token, tsekcija, true, locationcounter, true, false);
    if (symbols.find(s->name) == symbols.end())
    {
        s->number = counter;
        s->defined = true;
        counter++;
        symbols.insert(std::pair<string, Simbol>(token, *s));
        return true;
    }
    else
    {
        return false;
    }
}
Simbol *TabelaSimbola::dohvatiSimbol(string naziv)
{
    auto it = symbols.find(naziv);
    return &it->second;
}
void TabelaSimbola::dodajPom(Simbol *sim, int pozicije, char znaci, bool velicina, bool skok)
{

    sim->pomocna.push_back(*(new Pomocna(pozicije, znaci, velicina, skok)));
}
void TabelaSimbola::definisiSimbol(Simbol *simb, vector<Podaci *> sekcije, vector<Relokaciona *> rt, Obracanja *to)
{

    int broj = 0;
    bool lokalna = false;

    simb->defined = true;
    string trenutna = Asembler::tekucaSekcija;
    broj = simb->number;
    lokalna = simb->lokalna;

    for (int i = 0; i < simb->pomocna.size(); i++)
    {
        int poz = simb->pomocna.at(i).pozicije;
        char znak = simb->pomocna.at(i).znaci;
        bool jedanbajt = simb->pomocna.at(i).velicine;
        bool skok = simb->pomocna.at(i).skok;
        int noviOffset = simb->offset;

        if (skok)
        {

            vector<Relokacija> relokacije = rt[rt.size() - 1]->dohvatiRelokacije();
            for (int k = 0; k < relokacije.size(); k++)
            {
                if (relokacije[k].s.number == broj)
                {
                    noviOffset -= relokacije[i].s.offset;
                    break;
                }
            }
        }
        for (int k = 0; k < sekcije.size(); k++)
        {
            if (jedanbajt)
            {
                if (znak == '+')

                    sekcije[k]->promeniPodatak(poz, noviOffset);
                else
                {
                    sekcije[k]->promeniPodatak(poz, -noviOffset);
                }
            }
            else
            {
                if (znak == '+')
                {
                    sekcije[k]->promeniPodatak(poz, noviOffset, 2);
                }
                else
                {
                    sekcije[k]->promeniPodatak(poz, -noviOffset, 2);
                }
            }
        }
    }

    simb->pomocna.clear();
    if (lokalna)
    {
        for (int j = 0; j < rt.size(); j++)
        {

            int pomocna = dohvatiSimbol(rt[rt.size() - 1]->sekcija)->number;

            rt[j]->obrisiRelokaciju(broj, pomocna, rt[rt.size() - 1]->sekcija);
        }
    }
    to->obrisiObracanja(simb->name, sekcije, rt, to);
}

int TabelaSimbola::dohvatiOffset(string name, string sekcija)
{
    auto it = symbols.find(name);
    if (it != symbols.end())
    {
        if (it->second.sekcija == sekcija)
        {

            return 0;
        }
    }
    return -1;
}

void TabelaSimbola::stampaj(ofstream &out)
{
    out << "#id\t\time\t\tsekcija\t\ttip\t\toffset\t\tscope\t\tvelicina" << endl;
    for (auto &sym : symbols)
    {
        sym.second.stampaj(out);
        out << endl;
    }
}

bool TabelaSimbola::dalijesveDefinisano()
{
    for (auto const &sym : symbols)
    {
        if (sym.second.ex == false && sym.second.defined == false)
        {
            cout << "Simbol " << sym.second.name << " nije definisan!!" << endl;
            return false;
        }
        if (sym.second.ex == true && sym.second.defined == true)
        {
            cout << "Simbol " << sym.second.name << " ne sme biti definisan!!" << endl;
            return false;
        }
    }

    return true;
}

Simbol::Simbol(string name, string sekcija, bool sek, int offset, bool lokalna, bool ex)
{
    this->sek = sek;
    this->name = name;
    this->sekcija = sekcija;
    this->offset = offset;
    this->size = 0;
    this->lokalna = lokalna;
    this->number = 0;
    this->defined = false;
    this->isEqu = false;
    this->flag = 0;
    this->ex = ex;
}

void Simbol::stampaj(ofstream &out)
{
    string scope;
    if (lokalna)
        scope = "local";
    else
        scope = "global";

    string sType;
    if (sek)
        sType = "sekcija";
    else
        sType = "labela";

    out << number << "\t\t" << name << "\t\t" << sekcija << "\t\t" << sType << "\t\t" << offset << "\t\t" << scope << "\t\t" << size;
}
