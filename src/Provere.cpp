#include "Provere.h"
#include "TabelaSimbola.h"

map<string, int> Provere::brojOperanada =
    {
        {"CALL", 1},
        {"JMP", 1},
        {"JEQ", 1},
        {"JNE", 1},
        {"INT", 1},
        {"JGT", 1},

        {"PUSH", 1},
        {"POP", 1},
        {"ADD", 2},
        {"SUB", 2},
        {"TEST", 2},
        {"MUL", 2},
        {"DIV", 2},
        {"CMP", 2},

        {"XCHG", 2},
        {"AND", 2},
        {"OR", 2},
        {"SHL", 2},
        {"XOR", 2},
        {"NOT", 2},

        {"MOV", 2},

        {"SHR", 2},
        {"IRET", 0},
        {"RET", 0},
        {"HALT", 0}

};

map<string, int> Provere::OpCodeInstrukcije =
    {
        {"HALT", 0},
        {"IRET", 1},
        {"RET", 2},
        {"INT", 3},
        {"CALL", 4},
        {"JMP", 5},
        {"JEQ", 6},
        {"JNE", 7},
        {"JGT", 8},
        {"PUSH", 9},
        {"POP", 10},
        {"XCHG", 11},
        {"MOV", 12},
        {"ADD", 13},
        {"SUB", 14},
        {"MUL", 15},
        {"DIV", 16},
        {"CMP", 17},
        {"NOT", 18},
        {"AND", 19},
        {"OR", 20},
        {"XOR", 21},
        {"TEST", 22},
        {"SHL", 23},
        {"SHR", 24}

};

const regex Provere::symbol{("([a-zA-Z_][a-zA-Z0-9]*)")};
const regex Provere::zvzsimbol{("\\*([a-zA-Z_][a-zA-Z0-9]*)")};
const regex Provere::pravireg{"(r[0-7](h|l)?)"};
const regex Provere::zvezdicaprocenatregistar{"(\\*\\%r[0-7](h|l)?)"};
const regex Provere::zvzzagradapr{"(\\*\\(\\%r[0-7](h|l)?\\))"};
const regex Provere::zvezdarelpom{"(\\*)([a-zA-Z_][a-zA-Z0-9]*)\\(\\%r[0-7](h|l)?\\)"};
const regex Provere::zvezdarelpc{"(\\*)([a-zA-Z_][a-zA-Z0-9]*)\\(\\%pc\\)"};
const regex Provere::zvezdarelr7{"(\\*)([a-zA-Z_][a-zA-Z0-9]*)\\(\\%r7(h|l)?\\)"};
const regex Provere::zvezdabrojrelpom{"(\\*)([0-9]+)\\(\\%r[0-7](h|l)?\\)"};
const regex Provere::zvezdabrojrelpomhex{"(\\*)(0[xX][0-9a-fA-F]+)\\(\\%r[0-7](h|l)?\\)"};
const regex Provere::zvezdabrojrelpombin{"(\\*)0b([0-1]+)\\(\\%r[0-7](h|l)?\\)"};
const regex Provere::brojdeci{("([0-9]+)")};
const regex Provere::zvezdabrojdeci{("(\\*[0-9]+)")};
const regex Provere::zvezdabrojbin{("(\\*0b([0-1]+))")};
const regex Provere::zvezdabrojhex{("(\\*(0[xX][0-9a-fA-F]+))")};
const regex Provere::skinipc{("\\(\\%pc\\)")};
const regex Provere::skinireg{("\\(\\%r[0-7](h|l)?\\)")};
const regex Provere::brojbin{("0b([0-1]+)")};
const regex Provere::brojhex{("(0[xX][0-9a-fA-F]+)")};
const regex Provere::bukvregistarh{"(r[0-7]h)"};
const regex Provere::bukvregistarl{"(r[0-7]l)"};
const regex Provere::plusminus{"(\\+|\\-)"};

const regex Provere::instructionb{"(INT|ADD|SUB|MUL|DIV|CMP|AND|OR|NOT|TEST|PUSH|POP|CALL|IRET|MOV|SHL|SHR|RET|XOR|HALT|JMP|JEQ|JNE|JGT|XCHG)b"};
const regex Provere::instructionw{"(INT|ADD|SUB|MUL|DIV|CMP|AND|OR|NOT|TEST|PUSH|POP|CALL|IRET|MOV|SHL|SHR|RET|XOR|HALT|JMP|JEQ|JNE|JGT|XCHG)w"};
const regex Provere::instruction{"(INT|ADD|SUB|MUL|DIV|CMP|AND|OR|NOT|TEST|PUSH|POP|CALL|IRET|MOV|SHL|SHR|RET|XOR|HALT|JMP|JEQ|JNE|JGT|XCHG)(b|w)?"};
const regex Provere::label{"([a-zA-Z][a-zA-Z0-9_]*):"};
const regex Provere::pravasekcija{"(\\.)(text|data|bss)"};
const regex Provere::directive{"\\.(byte|word|skip|equ)"};
const regex Provere::sekcija{"(\\.section)"};
const regex Provere::section{"(\\.)(text|data|bss)\\:"};
const regex Provere::proveraPSW{"(%psw)"};

const regex Provere::dolarsimbol{"(\\$)([a-zA-Z_][a-zA-Z0-9]*)"};

const regex Provere::registar{"(\\%r[0-7](h|l)?)"};
const regex Provere::memreg{"(\\(\\%r[0-7](h|l)?\\))"};
const regex Provere::labelarelpom{"([a-zA-Z_][a-zA-Z0-9]*)\\(\\%r[0-7](h|l)?\\)"};
const regex Provere::relpcbezzvezdice{"([a-zA-Z_][a-zA-Z0-9]*)\\(\\%pc\\)"};
const regex Provere::dolarbrojdeci{("(\\$[0-9]+)")};
const regex Provere::dolarbrojhex{("(\\$(0[xX][0-9a-fA-F]+))")};
const regex Provere::dolarbrojbin{("(\\$0b([0-1]+))")};
const regex Provere::brojrelpom{"([0-9]+)\\(\\%(r[0-7]|pc)(h|l)?\\)"};
const regex Provere::hexbrojrelpom{"(0[xX][0-9a-fA-F]+)\\(\\%(r[0-7]|pc)(h|l)?\\)"};
const regex Provere::binbrojrelpom{"(0b([0-1]+))\\(\\%(r[0-7]|pc)(h|l)?\\)"};

bool Provere::proveriSkok(string token)
{

    if ((regex_match(token, Provere::symbol) && !regex_match(token, Provere::pravireg)) || regex_match(token, Provere::zvzzagradapr) ||
        regex_match(token, Provere::zvezdicaprocenatregistar) || regex_match(token, Provere::zvezdarelpom) || regex_match(token, Provere::zvezdarelpc) || regex_match(token, Provere::zvezdarelr7) || (regex_match(token, Provere::zvzsimbol) && !regex_match(token, Provere::pravireg)) || regex_match(token, Provere::zvezdabrojrelpom) || regex_match(token, Provere::zvezdabrojrelpombin) || regex_match(token, Provere::zvezdabrojrelpomhex) || regex_match(token, Provere::brojdeci) || regex_match(token, Provere::brojhex) || regex_match(token, Provere::brojbin) || regex_match(token, Provere::zvezdabrojbin) || regex_match(token, Provere::zvezdabrojhex) || regex_match(token, Provere::zvezdabrojdeci))
    {

        return true;
    }

    else
        return false;
}

bool Provere::dalijeSkok(string instrukcija)
{
    if (instrukcija == "CALL" || instrukcija == "INT" || instrukcija == "JMP" || instrukcija == "JGT" || instrukcija == "JEQ" || instrukcija == "JNE")
    {
        return true;
    }

    return false;
}

bool Provere::proveriPushPop(string instrukcija, string token)
{
    if (regex_match(token, proveraPSW))
    {
        return true;
    }
    else
    {
        if (instrukcija.compare("PUSH") == 0)
            return Provere::ispravanSRC(instrukcija, token);
        else if (instrukcija.compare("POP") == 0)
            return Provere::ispravanDST(instrukcija, token);
        else
            return false;
    }
}

bool Provere::ispravanSRC(string instrukcija, string token)
{

    if (
        (regex_match(token, Provere::symbol) && !regex_match(token, Provere::pravireg)) ||
        regex_match(token, Provere::registar) || regex_match(token, Provere::memreg) || regex_match(token, Provere::brojrelpom) || regex_match(token, Provere::hexbrojrelpom) || regex_match(token, Provere::binbrojrelpom) || regex_match(token, Provere::dolarsimbol) || regex_match(token, Provere::labelarelpom) || regex_match(token, Provere::relpcbezzvezdice) || regex_match(token, Provere::brojdeci) || regex_match(token, Provere::brojhex) || regex_match(token, Provere::brojbin) || regex_match(token, Provere::dolarbrojhex) || regex_match(token, Provere::dolarbrojbin) || regex_match(token, Provere::dolarbrojdeci))
    {

        return true;
    }

    else
        return false;
}
bool Provere::ispravanDST(string instrukcija, string token)
{

    if (
        (regex_match(token, Provere::symbol) && !regex_match(token, Provere::pravireg)) ||
        regex_match(token, Provere::registar) || regex_match(token, Provere::memreg) || regex_match(token, Provere::brojrelpom) || regex_match(token, Provere::hexbrojrelpom) || regex_match(token, Provere::binbrojrelpom) || regex_match(token, Provere::labelarelpom) || regex_match(token, Provere::relpcbezzvezdice) || regex_match(token, Provere::brojdeci) || regex_match(token, Provere::brojhex) || regex_match(token, Provere::brojbin))
    {

        return true;
    }

    else
        return false;
}

bool Provere::ispravanBroj(string token)
{

    return (regex_match(token, Provere::brojdeci) || regex_match(token, Provere::brojbin) || regex_match(token, Provere::brojhex));
}

bool Provere::dalijeJedBajt(string instrukcija, string token1, string token2)
{
    if (regex_match(token1, Provere::relpcbezzvezdice) || regex_match(token2, Provere::relpcbezzvezdice) || regex_match(token1, Provere::labelarelpom) || regex_match(token2, Provere::labelarelpom))
    {
        return false;
    }

    if (regex_search(token1, Provere::bukvregistarh) || regex_search(token1, Provere::bukvregistarl) || regex_search(token2, Provere::bukvregistarh) || regex_search(token2, Provere::bukvregistarl))
    {
        return true;
    }
    else if (regex_match(instrukcija, Provere::instructionb))
    {
        return true;
    }
    else
        return false;
}
int Provere::dalijevalidno(string instrukcija, string token1, string token2, string tokenzarez)
{

    if (regex_match(instrukcija, Provere::instructionw) && (regex_search(token1, Provere::bukvregistarh) || regex_search(token1, Provere::bukvregistarl) || regex_search(token2, Provere::bukvregistarh) || regex_search(token2, Provere::bukvregistarl)))
    {
        return -1;
    }
    if (tokenzarez != ",")
    {
        return -1;
    }
    return 0;
}

char Provere::dohvatiRegistar(string token)
{

    regex r{"(r[0-7])"};
    regex pc{"pc"};

    if (regex_search(token, r))
    {

        smatch match;
        regex_search(token, match, r);
        string instrukcija = match[1];

        string pomstring = instrukcija.substr(1, instrukcija.length());
        return stoi(pomstring);
    }
    else if (regex_search(token, pc))
    {
        return 7;
    }

    return -1;
}

char Provere::dohvatiVelicinu(string token)
{
    if (regex_match(token, Provere::bukvregistarh))
    {
        return 1;
    }
    else
        return 0;
}