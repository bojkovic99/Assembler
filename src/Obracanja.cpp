#include "Obracanja.h"
#include "Provere.h"

Obracanja::Obracanja(string sekcija, TabelaSimbola *ts)
{
    this->sekcija = sekcija;
    this->ts = ts;
}

Obracanje::Obracanje()
{

    this->brojaci.bss = 0;
    this->brojaci.text = 0;
    this->brojaci.data = 0;
    this->brojaci.und = 0;
}

void Obracanja::obrisiObracanja(string naziv, vector<Podaci *> sekcije, vector<Relokaciona *> rt, Obracanja *to)
{

    for (int i = 0; i < obracanja.size(); i++)
    {
        for (int j = obracanja[i].sadrzaj.size() - 1; j >= 0; j--)
        {

            if (obracanja[i].sadrzaj[j].simboli.name == naziv)
            {
                string znak = obracanja[i].sadrzaj[j].znaci;

                if (ts->dohvatiSimbol(naziv)->sekcija == ".text:")
                {

                    if (znak == "+")
                        obracanja[i].brojaci.text++;
                    else
                        obracanja[i].brojaci.text--;
                }
                else if (ts->dohvatiSimbol(naziv)->sekcija == ".bss:")
                {
                    if (znak == "+")
                        obracanja[i].brojaci.bss++;
                    else
                        obracanja[i].brojaci.bss--;
                }
                else if (ts->dohvatiSimbol(naziv)->sekcija == ".data:")
                {
                    if (znak == "+")
                        obracanja[i].brojaci.data++;
                    else
                        obracanja[i].brojaci.data--;
                }

                if (znak == "+")
                {
                    obracanja[i].value += ts->dohvatiSimbol(naziv)->offset;
                }
                else
                {
                    obracanja[i].value -= ts->dohvatiSimbol(naziv)->offset;
                }

                obracanja[i].brojaci.und--;

                proveraObracanja(obracanja[i].naziv, sekcije, rt, to);
            }
        }
    }
}

void Obracanja::ubaciObracanje(string naziv, string nobr, string znak, bool broj)
{

    for (int i = 0; i < obracanja.size(); i++)
    {
        if (obracanja[i].naziv == nobr)
        {
            if (broj == true)
            {
                if (regex_match(naziv, Provere::brojdeci))
                {

                    if (znak == "+")
                    {

                        obracanja[i].value += stoi(naziv);
                    }
                    else
                    {
                        obracanja[i].value -= stoi(naziv);
                    }
                    return;
                }
                else if (regex_match(naziv, Provere::brojhex))
                {
                    if (znak == "+")
                    {

                        obracanja[i].value += stoul(naziv, nullptr, 16);
                    }
                    else
                    {
                        obracanja[i].value -= stoul(naziv, nullptr, 16);
                    }
                    return;
                }
                else
                {

                    string pom2 = naziv.substr(2, naziv.size());

                    if (znak == "+")
                    {

                        obracanja[i].value += stoul(pom2, nullptr, 2);
                    }
                    else
                    {
                        obracanja[i].value -= stoul(pom2, nullptr, 2);
                    }
                    return;
                }
            }

            obracanja[i].sadrzaj.push_back(*(new Obr(*ts->dohvatiSimbol(naziv), znak)));
            Simbol *simb = ts->dohvatiSimbol(naziv);
            if (simb->defined)
            {
                if (ts->dohvatiSimbol(naziv)->sekcija == ".bss:")
                {
                    if (znak == "+")
                        obracanja[i].brojaci.bss++;
                    else
                        obracanja[i].brojaci.bss--;
                }

                if (ts->dohvatiSimbol(naziv)->sekcija == ".text:")
                {
                    if (znak == "+")
                        obracanja[i].brojaci.text++;
                    else
                        obracanja[i].brojaci.text--;
                }

                if (ts->dohvatiSimbol(naziv)->sekcija == ".data:")
                {
                    if (znak == "+")
                        obracanja[i].brojaci.data++;
                    else
                        obracanja[i].brojaci.data--;
                }

                if (znak == "+")
                {
                    obracanja[i].value += ts->dohvatiSimbol(naziv)->offset;
                }
                else
                {
                    obracanja[i].value -= ts->dohvatiSimbol(naziv)->offset;
                }
            }
            else
            {
                obracanja[i].brojaci.und++;
            }
        }
    }
}

void Obracanja::proveraObracanja(string proslediime, vector<Podaci *> sekcije, vector<Relokaciona *> rt, Obracanja *to)
{

    for (int i = 0; i < obracanja.size(); i++)
    {
        if (obracanja[i].naziv == proslediime)
        {
            Simbol *simb = ts->dohvatiSimbol(proslediime);

            if (obracanja[i].brojaci.text == 1 && obracanja[i].brojaci.data == 0 && obracanja[i].brojaci.bss == 0 && obracanja[i].brojaci.und == 0)
            {
                simb->offset = obracanja[i].value;
                ts->definisiSimbol(simb, sekcije, rt, to);
            }
            else if (obracanja[i].brojaci.text == 0 && obracanja[i].brojaci.data == 1 && obracanja[i].brojaci.bss == 0 && obracanja[i].brojaci.und == 0)
            {
                simb->offset = obracanja[i].value;
                ts->definisiSimbol(simb, sekcije, rt, to);
            }
            else if (obracanja[i].brojaci.text == 0 && obracanja[i].brojaci.data == 0 && obracanja[i].brojaci.bss == 1 && obracanja[i].brojaci.und == 0)
            {
                simb->offset = obracanja[i].value;
                ts->definisiSimbol(simb, sekcije, rt, to);
            }
            else if (obracanja[i].brojaci.text == 0 && obracanja[i].brojaci.data == 0 && obracanja[i].brojaci.bss == 0 && obracanja[i].brojaci.und == 0)
            {
                simb->offset = obracanja[i].value;
                ts->definisiSimbol(simb, sekcije, rt, to);
            }
        }
    }
}

bool Obracanja::dalijesveDefinisano()
{
    for (int i = 0; i < obracanja.size(); i++)
    {

        if (obracanja[i].brojaci.text == 1 && obracanja[i].brojaci.data == 0 && obracanja[i].brojaci.bss == 0 && obracanja[i].brojaci.und == 0)
        {
        }
        else if (obracanja[i].brojaci.text == 0 && obracanja[i].brojaci.data == 1 && obracanja[i].brojaci.bss == 0 && obracanja[i].brojaci.und == 0)
        {
        }
        else if (obracanja[i].brojaci.text == 0 && obracanja[i].brojaci.data == 0 && obracanja[i].brojaci.bss == 1 && obracanja[i].brojaci.und == 0)
        {
        }
        else if (obracanja[i].brojaci.text == 0 && obracanja[i].brojaci.data == 0 && obracanja[i].brojaci.bss == 0 && obracanja[i].brojaci.und == 0)
        {
        }
        else
        {
            return false;
        }
    }

    return true;
}