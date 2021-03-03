#include "Converter.h"

char Converter::ConInstrukcije(int podatak, int *locationcounter)
{
    (*locationcounter)++;

    podatak = podatak << 1;
    podatak += 1;
    return podatak << 2;
}
char Converter::ConInstrukcijeJedanBajt(int podatak, int *locationcounter)
{
    (*locationcounter)++;

    return (podatak << 3);
}