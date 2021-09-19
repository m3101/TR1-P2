/*
* Descrições mais completas disponíveis nos
* comentários do header.
* Este arquivo basicamente só contém código.
*/

#include "CamadaFisica.hpp"
#include <math.h>

double onda_base(double tempo,double periodo, double fase)
{
    return sin(2*M_PI*tempo/periodo+fase);
}

Demodulador::Demodulador(double fase,
                double periodo,
                double amplitude,
                double referencia,
                double delta_max,
                double alisamento)
{
    this->fase = fase;
    this->periodo = periodo;
    this->delta_max = delta_max;
    this->media_movel = 0;
    this->alisamento = alisamento;
    this->amplitude = amplitude;
    this->referencia = referencia;
    this->periodo = periodo;
    this->S = NEUTRO;
    this->tempo = 0;
    this->posedge = NULL;
    this->negedge = NULL;
    this->deltat = 1.0/30.0;
}

Demodulador::~Demodulador(){}

double Demodulador::_mult_amostra(double amostra)
{
    tempo+=deltat;
    tempo = fmod(tempo,periodo);
    return this->amplitude*onda_base(tempo,periodo,fase+M_PI/2)*amostra;
}

void Demodulador::_media_movel(double amostra)
{
    this->media_movel = this->media_movel*this->alisamento+amostra*(1-this->alisamento);
}

void Demodulador::_transicao(unsigned char trigger)
{
    switch(S)
    {
        case BAIXO:
        if(media_movel+referencia>delta_max)
        {
            S=NEUTRO;
            if(trigger && this->posedge)
                this->posedge(this,this->param);
        }
        break;
        case NEUTRO:
        if(media_movel>delta_max)
        {
            S=ALTO;
            if(trigger && this->posedge)
                this->posedge(this,this->param);
        }
        else if(media_movel<-delta_max)
        {
            S=BAIXO;
            if(trigger && this->negedge)
                this->negedge(this,this->param);
        }
        break;
        case ALTO:
        if(referencia-media_movel>delta_max)
        {
            S=NEUTRO;
            if(trigger && this->negedge)
                this->negedge(this,this->param);
        }
        break;
    }
}

double Demodulador::ajusta_fase(double amostra)
{
    _media_movel(_mult_amostra(amostra));
    if(amostra!=0 && media_movel<this->referencia)
    {
        this->fase+=.00752;
        this->fase = fmod(this->fase,M_PI*2);
    }
    _transicao(0);
    return fabs(media_movel-referencia);
}

void Demodulador::recebe_amostra(double amostra)
{
    _media_movel(_mult_amostra(amostra));
    _transicao(1);
}

Modulador::Modulador(double periodo,
                     double amplitude)
{
    this->amplitude = amplitude;
    this->periodo = periodo;
    this->S = ALTO;
    this->tempo = 0;
    this->deltat = 1.0/30.0;
}

void Modulador::passatempo()
{
    this->tempo+=this->deltat;
    this->tempo = fmod(this->tempo,this->periodo);
    double deltafase;
    switch (S)
    {   
        case NEUTRO:
            deltafase=0;
        break;
        case ALTO:
            deltafase=-M_PI/2;
        break;
        case BAIXO:
            deltafase=M_PI/2;
        break;
    }
    this->saida = this->amplitude*onda_base(this->tempo,this->periodo,deltafase);
}

Modulador::~Modulador(){}

Transmissor::Transmissor(double periodo, double amplitude, double bitrate)
{
    this->ajustando = 1;
    this->atual = 0;
    this->bit_atual = 0;
    this->fim = 0;
    this->p_atual = MANCHESTER;
    this->periodo_bit = 1/bitrate;
    this->deltat = 1.0/30.0;
    this->tempo = 0;
    this->clock = new Modulador(periodo,amplitude);
    this->sinal = new Modulador(periodo,amplitude);
}

Transmissor::~Transmissor()
{
    delete this->clock;
    delete this->sinal;
}

void Transmissor::putchar(char c)
{
    this->buffer[this->fim]=c;
    this->fim+=1;
}

void Transmissor::passatempo()
{
    unsigned char bit = (this->buffer[this->atual]&(1<<this->bit_atual))>>this->bit_atual;

    if(this->atual!=this->fim)
    {
        this->tempo += this->deltat;
        this->tempo = fmod(this->tempo,this->periodo_bit);
    }

    this->sinal->passatempo();
    this->clock->passatempo();

    /*
    Em qual quarto do período de envio de 1 bit estamos
    */
    unsigned char quarto = 4*this->tempo/this->periodo_bit;

    if(ajustando)
    {
        this->sinal->S=ALTO;
        this->clock->S=ALTO;
    }
    else
    {
        switch(this->p_atual)
        {
            case BINARIO:
                /*
                * Ciclo:
                * * Define bit de sinal (alto = 1, baixo = 0)
                * * Levanta o clock
                * * Abaixa o clock
                */
                switch (quarto)
                {
                    /*
                    * No primeiro terço (dois primeiros quartos)
                    * * o clock deve estar baixo
                    * * definimos o bit de sinal a transmitir
                    */
                    case 0:
                    case 1:
                        this->clock->S=BAIXO;
                        /*
                        * Sinal <= Bit atual
                        */
                        this->sinal->S=bit?ALTO:BAIXO;
                    break;
                    case 2:
                        /*
                        * Se há bits a transmitir
                        */
                        if(this->atual!=this->fim)
                            this->clock->S=ALTO;
                    break;
                    case 3:
                        /*
                        * Se ainda não abaixamos o clock, vamos para o próximo bit
                        * e abaixamos o clock
                        */
                    if(this->clock->S==ALTO)
                    {
                        this->clock->S=BAIXO;
                        this->bit_atual+=1;
                        if(this->bit_atual>=8)
                        {
                            this->bit_atual=0;
                            this->atual+=1;
                            this->tempo=0;
                        }
                    }
                    break;
                }
            break;
            case BIPOLAR:
                /*
                * Ciclo:
                * * Vai para o neutro
                * * Transmite um bit
                * * Vai para o neutro
                * * Transmite um bit
                */
                switch (quarto)
                {
                    case 0:
                        if(this->bit_atual>=8)
                        {
                            this->bit_atual=0;
                            this->atual+=1;
                            this->tempo=0;
                        }
                    case 2:
                    this->sinal->S=NEUTRO;
                    break;
                    case 3:
                    case 1:
                        if(this->sinal->S==NEUTRO)
                        {
                            this->sinal->S=bit?ALTO:BAIXO;
                            this->bit_atual+=1;
                        }
                    break;
                }
            break;
            case MANCHESTER:
                /*
                Para a codificação Manchester,
                introduzimos um delay para possibilitar a interpretação mais fácil.
                O ciclo é o seguinte:
                1) Clock->BAIXO
                2) Sinal->bit
                3) Clock->ALTO
                4) Sinal->bit ^ clock
                */
                switch (quarto)
                {
                    case 0:
                        this->clock->S = BAIXO;
                        if(this->bit_atual>=8)
                        {
                            this->bit_atual=0;
                            this->atual+=1;
                            this->tempo=0;
                        }
                    break;
                    case 1:
                        this->sinal->S = bit?ALTO:BAIXO;
                    break;
                    case 2:
                        this->clock->S = ALTO;
                    break;
                    case 3:
                        if(this->clock->S==ALTO)
                        {
                            this->clock->S=BAIXO;
                            this->sinal->S = bit?BAIXO:ALTO;
                            this->bit_atual+=1;
                        }
                    break;
                }
            break;
        }
    }

}

/*
As seguintes funções determinam o comportamento do receptor
em relação ao sinal de forma abstrata.
*/
void _receptor_on_posedge_sinal(void *self,void* receptor)
{
    Receptor *rec =(Receptor*) receptor;
    switch(rec->p_atual)
    {
        case BIPOLAR:
            if(rec->sinal->S==ALTO)
            {
                rec->byte |= 1<<rec->bit_atual;
                rec->bit_atual+=1;
            }
        break;
    }
}
void _receptor_on_negedge_sinal(void *self,void* receptor)
{
    Receptor *rec =(Receptor*) receptor;
    switch(rec->p_atual)
    {
        case BIPOLAR:
            if(rec->sinal->S==BAIXO)
            {
                rec->byte &= ~(1<<rec->bit_atual);
                rec->bit_atual+=1;
            }
        break;
    }
}
void _receptor_on_posedge_clock(void *self,void* receptor)
{
    Receptor *rec =(Receptor*) receptor;
    switch(rec->p_atual)
    {
        case BINARIO:
        case MANCHESTER:
            /*
            Na codificação binária, a cada subida do clock lemos
            diretamente o bit.
            Na codificação Manchester com delay, temos a mesma situação
            */
            if(rec->clock->S==ALTO)
            {
                rec->byte &= ~(1<<rec->bit_atual);
                rec->byte |= ((rec->sinal->S==ALTO)?1:0)<<rec->bit_atual;
                rec->bit_atual+=1;
            }
        break;
    }
}
void _receptor_on_negedge_clock(void *self,void* receptor)
{}

Receptor::Receptor(double periodo, double amplitude)
{
    this->ajustando=1;
    this->bit_atual=0;
    this->byte=255;
    this->clock = new Demodulador(0,periodo,amplitude,3.508,3.3,.4);
    this->clock->posedge = _receptor_on_posedge_clock;
    this->clock->negedge = _receptor_on_negedge_clock;
    this->clock->param = (void*)this;
    this->deltat = 1.0/30.0;
    this->on_char = NULL;
    this->p_atual = MANCHESTER;
    this->params = NULL;
    this->sinal = new Demodulador(0,periodo,amplitude,3.508,3.3,.4);
    this->sinal->posedge = _receptor_on_posedge_sinal;
    this->sinal->negedge = _receptor_on_negedge_sinal;
    this->sinal->param = (void*)this;
}

Receptor::~Receptor()
{
    delete this->sinal;
    delete this->clock;
}

void Receptor::passatempo(double clock, double sinal)
{
    if(this->bit_atual>=8)
    {
        this->bit_atual=0;
        if(this->on_char)
            this->on_char(this->byte,this->params);
    }
    if(ajustando)
    {
        double delta;
        delta_clock = this->clock->ajusta_fase(clock);
        delta_sinal =this->sinal->ajusta_fase(sinal);
    }
    else
    {
        this->clock->recebe_amostra(clock);
        this->sinal->recebe_amostra(sinal);
    }
}