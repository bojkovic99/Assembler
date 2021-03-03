#include "Asembler.h"
#include "Lexer.h"
#include "Provere.h"
#include "Converter.h"
#include "Relokaciona.h"
#include "Obracanja.h"
#include "Podaci.h"

Asembler::Asembler(ifstream &in, ofstream &out)
{
    this->in = &in;

    this->out = &out;

    ts = new TabelaSimbola();
    to = new Obracanja(".UND", ts);
    rt = new Relokaciona(".UND");
    s = new Podaci(0, ".UND");
    relokacione.push_back(rt);
    podaci.push_back(s);
};

void Asembler::generisiTokene()
{
    string line;
    int brojac = 0;
    bool greska = false;
    string poruka = "";
    while (getline(*in, line))
    {

        vector<string> tokeni = Lexer::napraviTokene(line);

        if (tekucaSekcija == ".end" && tokeni.size() != 0)
        {

            cout << endl
                 << line << endl
                 << "Greska na liniji broj " << brojac + 1 << endl;

            greska = true;
            break;
        }
        else if (tekucaSekcija != ".end" && obradiTokene(tokeni, &poruka) == false)
        {
            cout << poruka
                 << endl
                 << line << endl
                 << "Greka na liniji broj " << brojac + 1 << endl;

            greska = true;
            break;
        }

        brojac++;
    }

    if (tekucaSekcija != ".end" && !greska)
    {
        cout << "Greska! Ne postoji .end!";
        greska = true;
    }

    if (!greska)
    {
        if (dalijesveDefinisano())
        {
            ts->stampaj(*out);
            *out << " " << endl;

            for (int i = 0; i < relokacione.size(); i++)
            {
                relokacione[i]->stampaj(*out);
                *out << endl;
            }

            for (int i = 0; i < podaci.size(); i++)
            {
                podaci[i]->stampaj(*out);
                *out << endl;
            }
        }
        else
        {
            cout << "Nisu definisani svi simboli!";
        }
    }
}

string Asembler::tekucaSekcija = ".UND";

bool Asembler::obradiTokene(vector<string> tokeni, string *poruka)
{

    using namespace std;

    for (int i = 0; i < tokeni.size(); i++)
    {
        string ulaz = tokeni[i];

        if ((ulaz == ".global" || ulaz == ".extern") && tekucaSekcija == ".UND")
        {
            i++;

            int flag = 0;
            bool ext = (ulaz == ".extern");

            for (; i < tokeni.size(); i++)
            {
                if (flag == 0 && regex_match(tokeni[i], Provere::symbol))
                {
                    flag = 1;

                    if (!ts->dodajNoviSimbol(tokeni[i], ".UND", locationcounter, false, ext))
                    {
                        return false;
                    };
                }
                else if (flag == 1 && tokeni[i] == ",")
                {
                    flag = 0;
                }
                else
                {
                    *poruka = ulaz + " nije u dobrom formatu!";
                    return false;
                }
            }
            if (flag == 0)
            {
                *poruka = ulaz + " direktiva nije dovrsena!";
                return false;
            }
        }
        else if ((ulaz == ".global" || ulaz == ".extern") && tekucaSekcija != ".UND")
        {
            *poruka = "Global i extern se ne mogu da se nalaze van .UND sekcije";
            return false;
        }

        else if (ulaz == ".end")
        {
            string StaraSekcija = Asembler::tekucaSekcija;
            Asembler::tekucaSekcija = ulaz;
            int slc = locationcounter;

            povecajLocationcounter(-locationcounter);

            if (!ts->dodajNovuSekciju(tekucaSekcija, tekucaSekcija, locationcounter, StaraSekcija, slc))
            {

                return false;
            }
            i++;
            if (i < tokeni.size())
            {
                *poruka = "Nista se ne sme nalaziti posle .end";
                return false;
            }
            return true;
        }
        else if (regex_match(ulaz, Provere::label))
        {
            string pomocno = ulaz;

            pomocno = pomocno.substr(0, ulaz.length() - 1);

            if (!ts->dodajNoviSimbol(pomocno, tekucaSekcija, locationcounter, true))
            {
                Simbol *sim = ts->dohvatiSimbol(pomocno);
                if (sim->defined == true)
                {
                    *poruka = "Labela " + ulaz + " je vec definisana!";
                    return false;
                }

                sim->offset = locationcounter;
                sim->sekcija = tekucaSekcija;
            }
            Simbol *sim = ts->dohvatiSimbol(pomocno);
            if (sim->ex)
            {
                return false;
            }
            if (sim->defined == true)
            {
                return false;
            }

            ts->definisiSimbol(sim, podaci, relokacione, to);
        }
        else if (regex_match(ulaz, Provere::sekcija))
        {
            i++;
            if (i != tokeni.size() && regex_match(tokeni[i], Provere::section))
            {
                string StaraSekcija = Asembler::tekucaSekcija;
                int slc = locationcounter;
                Asembler::tekucaSekcija = tokeni[i];
                povecajLocationcounter(-locationcounter);

                Relokaciona *r = new Relokaciona(Asembler::tekucaSekcija);
                rt = r;

                relokacione.push_back(rt);

                int pomocna = s->counter;
                s = new Podaci(pomocna, Asembler::tekucaSekcija);
                podaci.push_back(s);
                to->sekcija = Asembler::tekucaSekcija;

                if (!ts->dodajNovuSekciju(tekucaSekcija, tekucaSekcija, locationcounter, StaraSekcija, slc))
                {
                    *poruka = "Sekcija sa nazivom" + tekucaSekcija + " je vec definisana!";
                    return false;
                }
            }
            else
                return false;
        }
        else if (regex_match(ulaz, Provere::directive))
        {
            if (Asembler::tekucaSekcija != ".data:" && Asembler::tekucaSekcija != ".text:" && Asembler::tekucaSekcija != ".bss:")
            {
                *poruka = "Direktive se ne mogu nalaziti van .data ili .text sekcije!";
                return false;
            }
            if (ulaz == ".equ")
            {
                if (tokeni[i + 2] != ",")
                {
                    *poruka = ".equ direktiva nije u dobrom formatu!";
                    return false;
                }

                ts->dodajNoviSimbol(tokeni[i + 1], tekucaSekcija, locationcounter, true);
                if (ts->dohvatiSimbol(tokeni[i + 1])->defined == true)
                {
                    *poruka = "Simbol koji treba da se definise .equ direktivom je vec definisan!";
                    return false;
                }
                string definisiIme = tokeni[i + 1];

                Obracanje *no = new Obracanje();
                no->naziv = definisiIme;
                to->obracanja.push_back(*no);

                i = i + 3;

                int flag = 0;

                string znak = "+";

                if (regex_match(tokeni[i], Provere::plusminus))
                {
                    znak = tokeni[i];
                    i++;
                }
                while (i < tokeni.size())
                {
                    if (flag == 0 && (regex_match(tokeni[i], Provere::symbol) || Provere::ispravanBroj(tokeni[i]) || regex_match(tokeni[i], Provere::pravasekcija)))
                    {
                        flag = 1;
                        if (!Provere::ispravanBroj(tokeni[i]) && !regex_match(tokeni[i], Provere::pravasekcija))
                        {

                            ts->dodajNoviSimbol(tokeni[i], tekucaSekcija, locationcounter, true);

                            to->ubaciObracanje(tokeni[i], definisiIme, znak, false);
                        }
                        else if (Provere::ispravanBroj(tokeni[i]))
                        {
                            to->ubaciObracanje(tokeni[i], definisiIme, znak, true);
                        }
                    }

                    else if (flag == 1 && regex_match(tokeni[i], Provere::plusminus))
                    {
                        znak = tokeni[i];
                        flag = 0;
                    }
                    else
                    {
                        *poruka = ".equ nije u dobrom formatu!";
                        return false;
                    }
                    i++;
                }
                to->proveraObracanja(definisiIme, podaci, relokacione, to);

                if (flag == 0)
                {

                    return false;
                }
            }
            else if (tokeni[i] == ".byte")
            {

                string prostor = tokeni[i];
                int flag = 0;
                i += 1;
                while (i < tokeni.size())
                {
                    if (regex_match(tokeni[i], Provere::plusminus))
                    {
                        i++;
                        continue;
                    }
                    string simbProvera = tokeni[i];
                    if ((regex_match(simbProvera, Provere::symbol) || regex_match(simbProvera, Provere::pravasekcija) || Provere::ispravanBroj(simbProvera)) && flag == 0)
                    {
                        flag = 1;
                        if (regex_match(simbProvera, Provere::symbol))
                        {

                            generisiPodatke(simbProvera);
                        }
                        else if (Provere::ispravanBroj(simbProvera))
                        {
                            string vredn = tokeni[i];
                            if (regex_match(tokeni[i - 1], Provere::plusminus))
                            {
                                if (tokeni[i - 1] == "-")
                                {
                                    tokeni[i] = "-" + tokeni[i];
                                }
                            }
                            if (regex_match(vredn, Provere::brojdeci))
                            {

                                s->dodajPodatak(stoi(tokeni[i]));
                            }
                            else if (regex_match(vredn, Provere::brojhex))
                            {

                                s->dodajPodatak(stoul(tokeni[i], nullptr, 16));
                            }
                            else
                            {

                                string pom2 = vredn.substr(2, vredn.size());
                                if (tokeni[i].at(0) == '-')
                                {
                                    pom2 = "-" + pom2;
                                }
                                s->dodajPodatak(stoul(pom2, nullptr, 2));
                            }
                        }
                        else
                        {
                            s->dodajPodatak(0);
                        }

                        povecajLocationcounter(1);
                    }
                    else if (flag == 1 && tokeni[i] == ",")
                    {
                        flag = 0;
                    }
                    else
                    {
                        *poruka = "Neispravan format" + ulaz;
                        return false;
                    }
                    i++;
                }
                if (flag == 0)
                {

                    return false;
                }
            }
            else if (tokeni[i] == ".word")
            {

                string prostor = tokeni[i];
                int flag = 0;

                i++;
                while (i < tokeni.size())
                {
                    if (regex_match(tokeni[i], Provere::plusminus))
                    {
                        i++;
                        continue;
                    }
                    if ((regex_match(tokeni[i], Provere::symbol) || Provere::ispravanBroj(tokeni[i]) || regex_match(tokeni[i], Provere::pravasekcija)) && flag == 0)
                    {
                        flag = 1;
                        if (regex_match(tokeni[i], Provere::symbol))
                        {

                            generisiPodatkeW(tokeni[i]);
                        }
                        else if (Provere::ispravanBroj(tokeni[i]))
                        {
                            string vredn = tokeni[i];
                            if (regex_match(tokeni[i - 1], Provere::plusminus))
                            {
                                if (tokeni[i - 1] == "-")
                                {
                                    tokeni[i] = "-" + tokeni[i];
                                }
                            }
                            if (regex_match(vredn, Provere::brojdeci))
                            {

                                s->dodajPodatak(stoi(tokeni[i]), 2);
                            }
                            else if (regex_match(vredn, Provere::brojhex))
                            {

                                s->dodajPodatak((stoul(tokeni[i], nullptr, 16)), 2);
                            }
                            else
                            {

                                string pom2 = vredn.substr(2, vredn.size() - 2);
                                if (tokeni[i].at(0) == '-')
                                {
                                    pom2 = "-" + pom2;
                                }

                                s->dodajPodatak(stoul(pom2, nullptr, 2), 2);
                            }
                        }
                        else
                        {

                            s->dodajPodatak(0, 2);
                        }

                        povecajLocationcounter(2);
                    }
                    else if (flag == 1 && tokeni[i] == ",")
                    {
                        flag = 0;
                    }
                    else
                    {
                        *poruka = "Neispravan format " + ulaz;
                        return false;
                    }
                    i++;
                }
                if (flag == 0)
                {

                    return false;
                }
            }
            else if (tokeni[i] == ".skip")
            {

                if (tokeni.size() != i + 2)
                {
                    *poruka = "Nekoretkna .skip direktiva";
                    return false;
                }
                i++;
                if (!Provere::ispravanBroj(tokeni[i]))
                {
                    *poruka = "Parametar .skip direktive nije literal!";
                    return false;
                }
                else
                {
                    if (regex_match(tokeni[i], Provere::brojdeci))
                    {
                        povecajLocationcounter(stoi(tokeni[i]));
                        int br = stoi(tokeni[i]);
                        for (int i = 0; i < br; i++)
                        {
                            s->dodajPodatak(0, 1);
                        }
                    }
                    else if (regex_match(tokeni[i], Provere::brojhex))
                    {

                        povecajLocationcounter(stoul(tokeni[i], nullptr, 16));

                        int pomocna = stoul(tokeni[i], nullptr, 16);
                        for (int i = 0; i < pomocna; i++)
                        {

                            s->dodajPodatak(0, 1);
                        }
                    }
                    else
                    {
                        string vredn = tokeni[i].substr(2, tokeni[i].size() - 2);
                        povecajLocationcounter(stoul(vredn, nullptr, 2));

                        int pomocna = stoul(vredn, nullptr, 2);
                        for (int i = 0; i < pomocna; i++)
                        {
                            s->dodajPodatak(0, 1);
                        }
                    }
                }
            }
            else
            {
                *poruka = "Inkorektna sintaksa direktive";
                return false;
            }
        }

        else if (regex_match(ulaz, Provere::instruction))
        {
            if (Asembler::tekucaSekcija != ".text:")
            {
                *poruka = "Instrukcije se nalaze u pogresnoj sekciji!";
                return false;
            }

            string instr = tokeni[i];
            poslinstrukcija = s->counter;
            int size = Provere::brojOperanada[instr];

            if (size == 0 && tokeni.size() - 1 == i)
            {
                char pomocno = Converter::ConInstrukcijeJedanBajt(Provere::OpCodeInstrukcije[instr], &locationcounter);
                s->dodajPodatak(pomocno);
            }
            else if (size == 1 && tokeni.size() - 1 - i == 1)
            {
                char pomocno = Converter::ConInstrukcije(Provere::OpCodeInstrukcije[instr], &locationcounter);
                s->dodajPodatak(pomocno);
                i = i + 1;
                if (Provere::proveriPushPop(instr, tokeni[i]))
                {

                    if (generisiKod(tokeni[i]))
                    {

                        return false;
                    }
                }

                else if (Provere::dalijeSkok(instr))
                {
                    Provere::proveriSkok(tokeni[i]);
                    if (generisiKodSkok(tokeni[i]))
                    {

                        return false;
                    }
                }
                else
                {
                    *poruka = "Neispravna jednoadresna funkcija!";
                    return false;
                }
            }
            else if (size == 2 && tokeni.size() - 1 - i == 3)
            {
                bool dalijeJesadnB = Provere::dalijeJedBajt(tokeni[i], tokeni[i + 1], tokeni[i + 3]);
                char pomocno = 0;
                if (dalijeJesadnB)
                {
                    pomocno = Converter::ConInstrukcijeJedanBajt(Provere::OpCodeInstrukcije[instr], &locationcounter);
                }
                else
                {
                    pomocno = Converter::ConInstrukcije(Provere::OpCodeInstrukcije[instr], &locationcounter);
                }

                s->dodajPodatak(pomocno);
                i = i + 3;

                if (Provere::dalijevalidno(tokeni[i - 3], tokeni[i - 2], tokeni[i], tokeni[i - 1]) == -1)
                {

                    *poruka = "Nije validnan format ";
                    return false;
                }

                if (instr == "shr" && (!Provere::ispravanSRC(instr, tokeni[i]) || !Provere::ispravanDST(instr, tokeni[i - 2])))
                {
                    *poruka = "Parametri dvoadresne instrukcije nisu dobrog oblika!";
                    return false;
                }
                else if (instr != "shr" && (!Provere::ispravanSRC(instr, tokeni[i - 2]) || !Provere::ispravanDST(instr, tokeni[i])))
                {
                    *poruka = "Parametri dvoadresne instrukcije nisu dobrog oblika!";
                    return false;
                }
                else
                {

                    if (instr == "shr")
                    {
                        if (generisiKod(tokeni[i]))
                        {
                            *poruka = "Nepoznat nacin adresiranja za prvi operand";
                            return false;
                        }
                        if (generisiKod(tokeni[i - 2]))
                        {
                            *poruka = "Nepoznat nacin adresiranja za drugi operand";
                            return false;
                        }
                    }
                    else
                    {

                        if (generisiKod(tokeni[i - 2]))
                        {
                            *poruka = "Nepoznat nacin adresiranja za drugi operand";
                            return false;
                        }

                        if (generisiKod(tokeni[i]))
                        {
                            *poruka = "Nepoznat nacin adresiranja za prvi operand";
                            return false;
                        }
                    }
                }
            }
            else
            {
                *poruka = "Nesipravno napisana instrukcija! ";
                return false;
            }
        }
        else
        {
            *poruka = "Neadekvatna instrukcija!";
            return false;
        }
    }
    return true;
}

bool Asembler::generisiKodSkok(string token)
{

    int x = 0;

    if (regex_match(token, Provere::symbol))
    {

        x = x << 5;
        s->dodajPodatak(x);
        povecajLocationcounter(1);
        generisiPodatkeW(token);
        povecajLocationcounter(2);
    }
    else if (regex_match(token, Provere::zvzsimbol))
    {
        x += 4;
        x = x << 5;
        s->dodajPodatak(x);
        povecajLocationcounter(1);
        string pom = token.substr(1, token.length() - 1);
        generisiPodatkeW(pom);
        povecajLocationcounter(2);
    }
    else if (regex_match(token, Provere::zvezdicaprocenatregistar))
    {
        x += 1;
        x = x << 4;
        char brojR = Provere::dohvatiRegistar(token);
        if (brojR == -1)
        {

            return true;
        }
        x += brojR;
        x = x << 1;
        x += Provere::dohvatiVelicinu(token);

        s->dodajPodatak(x);
        povecajLocationcounter(1);
    }
    else if (regex_match(token, Provere::zvzzagradapr))
    {
        x += 2;
        x = x << 4;
        char brojR = Provere::dohvatiRegistar(token);
        if (brojR == -1)
        {

            return true;
        }
        x += brojR;
        x = x << 1;
        x += Provere::dohvatiVelicinu(token);
        s->dodajPodatak(x);
        povecajLocationcounter(1);
    }
    else if (regex_match(token, Provere::zvezdarelpom))
    {
        string pomstring = token.substr(1, token.length());

        pomstring = regex_replace(pomstring, Provere::skinipc, "");
        pomstring = regex_replace(pomstring, Provere::skinireg, "");
        x += 3;
        x = x << 4;
        char brojR = Provere::dohvatiRegistar(token);

        if (brojR == -1)
        {

            return true;
        }
        x += brojR;
        x = x << 1;
        x += Provere::dohvatiVelicinu(token);
        s->dodajPodatak(x);
        povecajLocationcounter(1);
        if (brojR == 7)
        {
            generisiPodatkeSkok(pomstring);
        }
        else
        {
            generisiPodatkeW(pomstring);
        }

        povecajLocationcounter(2);
    }
    else if (regex_match(token, Provere::zvezdarelpc))
    {
        string pomstring = token.substr(1, token.length());
        pomstring = regex_replace(pomstring, Provere::skinipc, "");
        pomstring = regex_replace(pomstring, Provere::skinireg, "");

        x += 3;
        x = x << 4;
        x += 7;
        x = x << 1;

        s->dodajPodatak(x);
        povecajLocationcounter(1);
        generisiPodatkeSkok(pomstring);
        povecajLocationcounter(2);
    }
    else if (regex_match(token, Provere::zvezdabrojrelpom))
    {
        x += 3;
        x = x << 4;
        char brojR = Provere::dohvatiRegistar(token);
        if (brojR == -1)
        {

            return true;
        }
        x += brojR;
        x = x << 1;
        x += Provere::dohvatiVelicinu(token);
        s->dodajPodatak(x);
        povecajLocationcounter(1);
        string pomstring = token.substr(1, token.length() - 1);
        pomstring = regex_replace(pomstring, Provere::skinireg, "");
        s->dodajPodatak(stoi(pomstring), 2);
        povecajLocationcounter(2);
    }
    else if (regex_match(token, Provere::zvezdabrojrelpombin))
    {
        x += 3;
        x = x << 4;
        char brojR = Provere::dohvatiRegistar(token);
        if (brojR == -1)
        {

            return true;
        }
        x += brojR;
        x = x << 1;
        x += Provere::dohvatiVelicinu(token);
        s->dodajPodatak(x);
        povecajLocationcounter(1);
        string pomstring = token.substr(1, token.length() - 1);
        pomstring = regex_replace(pomstring, Provere::skinireg, "");
        pomstring = pomstring.substr(2, pomstring.size() - 2);

        s->dodajPodatak((stoul(pomstring, nullptr, 2)), 2);
        povecajLocationcounter(2);
    }
    else if (regex_match(token, Provere::zvezdabrojrelpomhex))
    {
        x += 3;
        x = x << 4;
        char brojR = Provere::dohvatiRegistar(token);
        if (brojR == -1)
        {

            return true;
        }
        x += brojR;
        x = x << 1;
        x += Provere::dohvatiVelicinu(token);
        s->dodajPodatak(x);
        povecajLocationcounter(1);
        string pomstring = token.substr(1, token.length() - 1);
        pomstring = regex_replace(pomstring, Provere::skinireg, "");
        pomstring = pomstring.substr(2, pomstring.length() - 2);
        pomstring = "0x0" + pomstring;
        s->dodajPodatak(stoi(pomstring, nullptr, 16), 2); // stoi - stoul
        povecajLocationcounter(2);
    }
    else if (regex_match(token, Provere::brojdeci))
    {
        x = 0;
        s->dodajPodatak(x);
        povecajLocationcounter(1);
        s->dodajPodatak(stoi(token), 2);
        povecajLocationcounter(2);
    }
    else if (regex_match(token, Provere::brojhex))
    {
        x = 0;
        s->dodajPodatak(x);
        povecajLocationcounter(1);
        string pomstring = token.substr(2, token.length() - 2);
        pomstring = "0x0" + pomstring;

        s->dodajPodatak(stoul(pomstring, nullptr, 16), 2);
        povecajLocationcounter(2);
    }
    else if (regex_match(token, Provere::brojbin))
    {
        x = 0;
        s->dodajPodatak(x);
        povecajLocationcounter(1);
        string pomstring = token.substr(2, token.length() - 2);
        s->dodajPodatak(stoul(token, nullptr, 2), 2);
        povecajLocationcounter(2);
    }
    else if (regex_match(token, Provere::zvezdabrojdeci))
    {
        string pomstring = token.substr(1, token.length() - 1);
        x = 4;
        x = x << 5;
        s->dodajPodatak(x);
        povecajLocationcounter(1);
        s->dodajPodatak(stoi(pomstring), 2);
        povecajLocationcounter(2);
    }
    else if (regex_match(token, Provere::zvezdabrojhex))
    {
        string pomstring = token.substr(1, token.length() - 1);
        x = 4;
        x = x << 5;
        s->dodajPodatak(x);
        povecajLocationcounter(1);
        pomstring = pomstring.substr(2, pomstring.length() - 2);
        pomstring = "0x0" + pomstring;
        s->dodajPodatak(stoul(pomstring, nullptr, 16), 2);
        povecajLocationcounter(2);
    }
    else if (regex_match(token, Provere::zvezdabrojbin))
    {
        string pomstring = token.substr(1, token.length() - 1);
        x = 4;
        x = x << 5;
        s->dodajPodatak(x);
        povecajLocationcounter(1);
        pomstring = pomstring.substr(2, pomstring.size() - 2);

        s->dodajPodatak((stoul(pomstring, nullptr, 2)), 2);
        povecajLocationcounter(2);
    }

    else
    {
        return true;
    }

    return false;
}
bool Asembler::generisiKod(string token)
{

    int x = 0;

    if (regex_match(token, Provere::symbol) && !regex_match(token, Provere::pravireg))
    {
        x += 4;
        x = x << 5;
        s->dodajPodatak(x);
        povecajLocationcounter(1);
        if ((s->uzmiPodatak(poslinstrukcija) & 4) != 0)
        {
            generisiPodatkeW(token);
            povecajLocationcounter(2);
        }
        else
        {
            generisiPodatke(token);
            povecajLocationcounter(1);
        }
    }
    else if (regex_match(token, Provere::dolarsimbol))
    {
        string pom = token;
        pom = pom.substr(1, token.length());

        x = x << 5;
        s->dodajPodatak(x);
        povecajLocationcounter(1);
        if ((s->uzmiPodatak(poslinstrukcija) & 4) != 0)
        {
            generisiPodatkeW(pom);
            povecajLocationcounter(2);
        }
        else
        {
            generisiPodatke(pom);
            povecajLocationcounter(1);
        }
    }
    else if (regex_match(token, Provere::brojrelpom))
    {
        string pomstring = token.substr(0, token.length());
        pomstring = regex_replace(pomstring, Provere::skinipc, "");
        pomstring = regex_replace(pomstring, Provere::skinireg, "");
        x += 3;
        x = x << 4;
        char brojR = Provere::dohvatiRegistar(token);
        if (brojR == -1)
        {

            return true;
        }
        x += brojR;
        x = x << 1;
        x += Provere::dohvatiVelicinu(token);
        s->dodajPodatak(x);
        povecajLocationcounter(1);
        s->dodajPodatak(stoi(pomstring), 2);
        povecajLocationcounter(2);
    }
    else if (regex_match(token, Provere::hexbrojrelpom))
    {
        string pomstring = token.substr(0, token.length());
        pomstring = regex_replace(pomstring, Provere::skinipc, "");
        pomstring = regex_replace(pomstring, Provere::skinireg, "");
        x += 3;
        x = x << 4;
        char brojR = Provere::dohvatiRegistar(token);
        if (brojR == -1)
        {

            return true;
        }
        x += brojR;
        x = x << 1;
        x += Provere::dohvatiVelicinu(token);
        s->dodajPodatak(x);
        povecajLocationcounter(1);
        pomstring = pomstring.substr(2, pomstring.length() - 2);
        pomstring = "0x0" + pomstring;
        s->dodajPodatak(stoul(pomstring, nullptr, 16), 2);
        povecajLocationcounter(2);
    }
    else if (regex_match(token, Provere::binbrojrelpom))
    {
        string pomstring = token.substr(0, token.length());
        pomstring = regex_replace(pomstring, Provere::skinipc, "");
        pomstring = regex_replace(pomstring, Provere::skinireg, "");
        x += 3;
        x = x << 4;
        char brojR = Provere::dohvatiRegistar(token);
        if (brojR == -1)
        {

            return true;
        }
        x += brojR;
        x = x << 1;
        x += Provere::dohvatiVelicinu(token);
        s->dodajPodatak(x);
        povecajLocationcounter(1);

        pomstring = pomstring.substr(2, pomstring.size() - 2);

        s->dodajPodatak((stoul(pomstring, nullptr, 2)), 2);
        povecajLocationcounter(2);
    }

    else if (regex_match(token, Provere::relpcbezzvezdice) || regex_match(token, Provere::labelarelpom))
    {
        string pomstring = token.substr(0, token.length());
        pomstring = regex_replace(pomstring, Provere::skinipc, "");
        pomstring = regex_replace(pomstring, Provere::skinireg, "");
        x += 3;
        x = x << 4;
        char brojR = Provere::dohvatiRegistar(token);
        if (brojR == -1)
        {

            return true;
        }
        x += brojR;
        x = x << 1;
        x += Provere::dohvatiVelicinu(token);
        s->dodajPodatak(x);
        povecajLocationcounter(1);
        if (brojR != 7)
        {
            generisiPodatkeW(pomstring);
            povecajLocationcounter(2);
        }
        else
        {
            generisiPodatkeSkok(pomstring);
            povecajLocationcounter(2);
        }
    }

    else if (regex_match(token, Provere::brojdeci))
    {
        x = 4;
        s->dodajPodatak(x);
        povecajLocationcounter(1);
        if ((s->uzmiPodatak(poslinstrukcija) & 4) != 0)
        {
            s->dodajPodatak(stoi(token), 2);
            povecajLocationcounter(2);
        }
        else
        {
            s->dodajPodatak(stoi(token));
            povecajLocationcounter(1);
        }
    }

    else if (regex_match(token, Provere::brojhex))
    {
        x = 4;
        s->dodajPodatak(x);
        povecajLocationcounter(1);
        if ((s->uzmiPodatak(poslinstrukcija) & 4) != 0)
        {

            s->dodajPodatak(stoul(token, nullptr, 16), 2);
            povecajLocationcounter(2);
        }
        else
        {
            s->dodajPodatak(stoul(token, nullptr, 16));
            povecajLocationcounter(1);
        }
    }

    else if (regex_match(token, Provere::brojbin))
    {
        x = 4;
        s->dodajPodatak(x);
        povecajLocationcounter(1);
        string pomstring = token;
        if ((s->uzmiPodatak(poslinstrukcija) & 4) != 0)
        {
            pomstring = pomstring.substr(2, pomstring.size() - 2);
            s->dodajPodatak(stoul(pomstring, nullptr, 2), 2);
            povecajLocationcounter(2);
        }
        else
        {
            pomstring = pomstring.substr(2, pomstring.size() - 2);
            s->dodajPodatak(stoul(pomstring, nullptr, 2));
            povecajLocationcounter(1);
        }
    }
    else if (regex_match(token, Provere::dolarbrojdeci))
    {
        string pomstring = token.substr(1, token.length() - 1);

        s->dodajPodatak(x);
        povecajLocationcounter(1);
        if ((s->uzmiPodatak(poslinstrukcija) & 4) != 0)
        {
            s->dodajPodatak(stoi(pomstring), 2);
            povecajLocationcounter(2);
        }
        else
        {
            s->dodajPodatak(stoi(pomstring));
            povecajLocationcounter(1);
        }
    }
    else if (regex_match(token, Provere::dolarbrojhex))
    {
        string pomstring = token.substr(1, token.length() - 1);

        s->dodajPodatak(x);
        povecajLocationcounter(1);
        if ((s->uzmiPodatak(poslinstrukcija) & 4) != 0)
        {
            s->dodajPodatak(stoul(pomstring, nullptr, 16), 2);
            povecajLocationcounter(2);
        }
        else
        {
            s->dodajPodatak(stoul(pomstring, nullptr, 16));
            povecajLocationcounter(1);
        }
    }
    else if (regex_match(token, Provere::dolarbrojbin))
    {

        s->dodajPodatak(x);
        povecajLocationcounter(1);
        if ((s->uzmiPodatak(poslinstrukcija) & 4) != 0)
        {
            string pomstring = token.substr(3, token.size() - 3);

            s->dodajPodatak((stoul(pomstring, nullptr, 2)), 2);
            povecajLocationcounter(2);
        }
        else
        {
            string pomstring = token.substr(3, token.size() - 3);
            s->dodajPodatak((stoul(pomstring, nullptr, 2)), 1);
            povecajLocationcounter(1);
        }
    }
    else if (regex_match(token, Provere::registar))
    {
        x = 1;
        x = x << 4;
        char brojR = Provere::dohvatiRegistar(token);
        if (brojR == -1)
        {

            return true; // true
        }
        x += brojR;
        x = x << 1;
        x += Provere::dohvatiVelicinu(token);
        s->dodajPodatak(x);
        povecajLocationcounter(1);
    }
    else if (regex_match(token, Provere::proveraPSW))
    {
        x = 1;
        x = x << 4;

        x += 15;
        x = x << 1;
        x += Provere::dohvatiVelicinu(token);
        s->dodajPodatak(x);
        povecajLocationcounter(1);
    }
    else if (regex_match(token, Provere::memreg))
    {
        x = 2;
        x = x << 4;
        char brojR = Provere::dohvatiRegistar(token);
        if (brojR == -1)
        {

            return true;
        }
        x += brojR;
        x = x << 1;
        x += Provere::dohvatiVelicinu(token);
        s->dodajPodatak(x);
        povecajLocationcounter(1);
    }
    else
    {
        return true;
    }

    return false;
}

void Asembler::generisiPodatke(string token)
{

    ts->dodajNoviSimbol(token, tekucaSekcija, locationcounter, true);
    Simbol *simb = ts->dohvatiSimbol(token);
    if (!simb->lokalna)
    {

        s->dodajPodatak(0);
        rt->dodajRelokaciju(locationcounter, RelType::R_386_16, *ts->dohvatiSimbol(token), tekucaSekcija);
    }

    else if (simb->lokalna)
    {

        if (simb->defined)
        {
            int vredn = ts->dohvatiOffset(token, tekucaSekcija);
            int vrednost = simb->offset;

            if (vredn == -1)
            {

                s->dodajPodatak(vrednost);
                rt->dodajRelokaciju(locationcounter, RelType::R_386_16, *ts->dohvatiSimbol(simb->sekcija), tekucaSekcija);
            }
            else
            {

                int vredn = vrednost;

                s->dodajPodatak(vredn);
            }
        }
        else
        {

            ts->dodajPom(simb, s->counter, '+', true);
            rt->dodajRelokaciju(locationcounter, RelType::R_386_16, *simb, tekucaSekcija);

            s->dodajPodatak(0, 2);
        }
    }
}
bool Asembler::dalijesveDefinisano()
{
    bool k1 = ts->dalijesveDefinisano();
    bool k2 = to->dalijesveDefinisano();
    bool k3 = k1 & k2;
    return k3;
}
void Asembler::generisiPodatkeW(string token)
{

    ts->dodajNoviSimbol(token, tekucaSekcija, locationcounter, true);
    Simbol *simb = ts->dohvatiSimbol(token);
    if (!simb->lokalna)
    {

        s->dodajPodatak(0, 2);
        rt->dodajRelokaciju(locationcounter, RelType::R_386_16, *ts->dohvatiSimbol(token), tekucaSekcija);
    }

    else if (simb->lokalna)
    {

        if (simb->defined == true) // Simbol je definisan
        {
            int vredn = ts->dohvatiOffset(token, tekucaSekcija);
            //
            int vrednost = simb->offset;

            if (vredn == -1) // definisan ali ne u nasoj sekciji
            {

                s->dodajPodatak(vrednost, 2);
                rt->dodajRelokaciju(locationcounter, RelType::R_386_16, *ts->dohvatiSimbol(simb->sekcija), tekucaSekcija);
            }
            else // definisan i u nasoj sekciji
            {

                int vredn = vrednost;
                s->dodajPodatak(vredn, 2);
            }
        }
        else // Simbol nije definisan
        {

            ts->dodajPom(simb, s->counter, '+', false);
            rt->dodajRelokaciju(locationcounter, RelType::R_386_16, *simb, tekucaSekcija);
            s->dodajPodatak(0, 2);
        }
    }
}

void Asembler::generisiPodatkeSkok(string token)
{

    ts->dodajNoviSimbol(token, tekucaSekcija, locationcounter, true);
    Simbol *simb = ts->dohvatiSimbol(token);
    if (!simb->lokalna)
    {

        s->dodajPodatak(-2, 2);

        rt->dodajRelokaciju(locationcounter, RelType::R_386_PC16, *ts->dohvatiSimbol(token), tekucaSekcija);
    }
    else if (simb->lokalna)
    {

        if (simb->defined)
        {
            int vredn = ts->dohvatiOffset(token, tekucaSekcija);
            int vrednost = simb->offset;

            if (vredn == -1)
            {

                s->dodajPodatak(vrednost - 2, 2);
                rt->dodajRelokaciju(locationcounter, RelType::R_386_PC16, *ts->dohvatiSimbol(simb->sekcija), tekucaSekcija);
            }
            else
            {

                int vredn = vrednost;

                s->dodajPodatak(vredn - (locationcounter + 2), 2);
            }
        }
        else
        {

            ts->dodajPom(simb, s->counter, '+', false, true);
            rt->dodajRelokaciju(locationcounter, RelType::R_386_PC16, *simb, tekucaSekcija);
            s->dodajPodatak(-2, 2);
        }
    }
}

void Asembler::povecajLocationcounter(int broj)
{
    locationcounter += broj;
}
