#include <stdio.h>
#include <math.h>
#include "CamadaFisica.hpp"


const char* names[]= {"BAIXO","NEUTRO","ALTO"};

int main()
{
    Modulador* envia = new Modulador(0.3,3);
    Demodulador* recebe = new Demodulador(M_PI,.3,3,4,3,.9);
    envia->S = ALTO;

    const estado stateprog[] = {ALTO,NEUTRO,BAIXO,ALTO};
    for(int i=0;i<30*4;i++)
    {
        envia->S = stateprog[i/30];
        envia->passatempo();
        recebe->recebe_amostra(envia->saida);
        printf("######\nSinal\tMedia\tFase\n%.3lf\t%.3lf\t%.3lf\nTempoE\tTempoR\t\n%.3lf\t%.3lf\n",
                envia->saida,
                recebe->media_movel,
                recebe->fase,
                envia->tempo,
                recebe->tempo);
        printf("S_e\tS_r\n%s\t%s\n",names[envia->S],names[recebe->S]);
    }

    delete envia;
    delete recebe;
}