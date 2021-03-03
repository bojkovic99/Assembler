#include <iostream>
#include <fstream>
#include <string>
#include "Asembler.h"

using namespace std;
int main(int argc, char **argv)
{
    if (argc != 3)
    {
        cout << "Pogresan broj operanada!" << endl;
        exit(-1);
    }

    string output = argv[1];
    string input = argv[2];

    ifstream ins;
    ofstream outs;
    ins.open(input);
    outs.open(output);
    cout << "Uspesno otvorio fajlove!" << endl;

    Asembler a = Asembler(ins, outs);

    try
    {
        a.generisiTokene();
    }
    catch (exception e)
    {
        cout << e.what() << endl;
    }
    if (ins.is_open())
    {
        ins.close();
    }
    if (outs.is_open())
    {
        outs.close();
    }

    return 0;
}