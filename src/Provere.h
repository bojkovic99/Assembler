#ifndef provere_h
#define provere_h
#include <iostream>
#include <vector>
#include <regex>
#include <string>
#include <map>

using namespace std;

class Provere
{

public:
    static const regex symbol;
    static const regex zvzsimbol;
    static const regex pravireg;
    static const regex zvezdicaprocenatregistar;
    static const regex zvzzagradapr;
    static const regex zvezdarelpom;
    static const regex zvezdarelpc;
    static const regex zvezdarelr7;
    static const regex zvezdabrojrelpom;
    static const regex zvezdabrojrelpomhex;
    static const regex zvezdabrojrelpombin;
    static const regex brojdeci;
    static const regex zvezdabrojdeci;
    static const regex zvezdabrojhex;
    static const regex zvezdabrojbin;
    static const regex skinipc;
    static const regex skinireg;
    static const regex label;
    static const regex pravasekcija;
    static const regex directive;
    static const regex sekcija;
    static const regex section;

    static const regex dolarsimbol;
    static const regex plusminus;
    static const regex registar;
    static const regex memreg;
    static const regex labelarelpom;
    static const regex relpcbezzvezdice;
    static const regex dolarbrojdeci;
    static const regex dolarbrojhex;
    static const regex dolarbrojbin;
    static const regex brojrelpom;
    static const regex bukvregistarh;
    static const regex bukvregistarl;
    static const regex instructionb;
    static const regex instructionw;
    static const regex instruction;
    static const regex hexbrojrelpom;
    static const regex binbrojrelpom;
    static const regex proveraPSW;

    static const regex brojbin;
    static const regex brojhex;
    static map<string, int> brojOperanada;
    static map<string, int> OpCodeInstrukcije;
    static bool proveriSkok(string token);
    static bool proveriPushPop(string instrukcija, string token);

    static bool ispravanSRC(string instrukcija, string token);
    static bool ispravanDST(string instrukcija, string token);
    static bool ispravanBroj(string token);

    static bool dalijeSkok(string instrukcija);
    static bool dalijeJedBajt(string instrukcija, string token1, string token2);
    static int dalijevalidno(string instrukcija, string token1, string token2, string tokenzarez);

    static char dohvatiRegistar(string token);
    static char dohvatiVelicinu(string token);
};

#endif