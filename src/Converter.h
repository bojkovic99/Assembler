#ifndef converter_h
#define converter_h
#include <iostream>
#include <vector>

using namespace std;

class Converter
{

public:
    static char ConInstrukcijeJedanBajt(int podatak, int *locationcounter);
    static char ConInstrukcije(int podatak, int *locationcounter);
};

#endif