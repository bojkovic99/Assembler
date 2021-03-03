#include "Relokaciona.h"
#include <fstream>

Relokaciona::Relokaciona(string sekcija)
{
    this->sekcija = sekcija;
}

Relokacija::Relokacija(short offset, RelType relT, Simbol &s, string sekcija) : s(s)
{
    this->offset = offset;
    this->relT = relT;
    this->sekcija = sekcija;
}
string Relokacija::dohvatiHexOffset() const
{
    char str[10];
    sprintf(str, "%04X", offset);
    string s(str);
    return s;
}
string Relokacija::dohvatiRelType() const
{
    switch (relT)
    {
    case RelType::R_386_16:
        return "R_386_16";
    case RelType::R_386_PC16:
        return "R_386_PC16";
    default:
        return "GRESKA";
    }
}
void Relokaciona::obrisiRelokaciju(int broj, int ts, string sek)
{

    for (int i = relokacije.size() - 1; i >= 0; i--)
    {
        if (relokacije[i].s.number == broj)
        {
            if (sek == relokacije[i].sekcija)
            {
                relokacije.erase(relokacije.begin() + i);
            }
            else
            {
                relokacije[i].s.number = ts;
            }
        }
    }
}

void Relokacija::stampaj(ofstream &out)
{
    out << dohvatiHexOffset() << "\t\t" << dohvatiRelType() << "\t\t" << s.number;
}
void Relokaciona::stampaj(ofstream &out)
{
    out << "#.ret" << sekcija << endl;
    out << "#offset\t\t#type\t\tvr[" << sekcija << "]" << endl;
    for (Relokacija r : relokacije)
    {
        r.stampaj(out);
        out << endl;
    }
    out << endl;
}

void Relokaciona::dodajRelokaciju(short offset, RelType relT, Simbol &s, string sekcija)
{
    Relokacija *rel = new Relokacija(offset, relT, s, sekcija);
    relokacije.push_back(*rel);
}