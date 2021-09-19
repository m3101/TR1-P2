#include <vector>
#include <ncurses.h>
#include "CamadaFisica.hpp"
#include <math.h>
#include <time.h>
#include <string.h>

/*
Para a GUI manter-se em uma taxa mais baixa de frames,
precisamos de uma função para esperar um intervalo de tempo
*/
#ifdef _WIN32
    #include <windows.h>
    int sleep(int t)
    {
        return Sleep(t);
    }
#else
    #include <unistd.h>
    int sleep(int t)
    {
        return usleep(1000*t);
    }
#endif // _WIN32

/*
Para administrar a nossa tela.
*/
char* nomes_protocolos[] = {"BINARIO","MANCHESTER","BIPOLAR"};
unsigned char tamanhos_protocolos[] = {7,10,7};
char* nomes_estados[] = {"BAIXO","NEUTRO","ALTO"};
unsigned char tamanhos_estados[] = {5,6,4};
char* nomes_modos[] = {"TRANSMITINDO","RECEBENDO","AJUSTANDO FASE"};
unsigned char tamanhos_modos[] = {12,9,14};

/*
O tamanho máximo de um texto a mostrar
*/
#define TAMANHOTEXTO 100
class Tela
{
    private:
    public:
    unsigned int w,h;
    /*Telas e subtelas do curses*/
    WINDOW *screen,*signal_w,*clock_w,
            *sender_w,*receiver_w,
            *text_sender_w,*text_receiver_w,
            *itext_sender_w,*itext_receiver_w;
    /*
    Evento de pressionar uma tecla
    e seu parâmetro
    */
    void (*on_key)(unsigned char key,void* param);
    void* param;
    /*
    Linhas de transmissão
    */
    std::vector<double> linha_sinal;
    std::vector<double> linha_clock;
    /*
    Nome do protocolo atual
    */
    char protocolo_atual;
    /*
    Nomes dos estados a mostrar
    */
    char estado_transmissor;
    char estado_receptor;
    char estado_clock;
    char estado_clock_receptor;
    /*
    Modo a mostrar
    */
    char modo_transmissor;
    char modo_receptor;

    /*
    Médias móveis a mostrar
    */
    double media_clock;
    double media_sinal;

    /*
    Byte do receptor
    */
    unsigned char byte_receptor;
    /*
    Bit atual
    */
    unsigned char bit_atual;
    /*
    Textos a mostrar
    */
    unsigned char *texto_transmissor;
    unsigned char texto_receptor[TAMANHOTEXTO];

    /*Máximos*/
    double maxC;
    double maxS;


    /*
    Tempo do início da simulação
    */
    unsigned long tempo_inicio;

    /*
    Imprime a tela
    */
    void imprime();

    Tela();
    ~Tela();
};

Tela::Tela()
{
    /*
    Configura as telas do curses
    */
    this->screen = initscr();
    maxS=0;
    maxC=0;
    noecho();
    keypad(screen,1);
    timeout(0);

    getmaxyx(screen,h,w);
    signal_w = subwin(screen,(h-1)/3,w,2*(h-1)/3+2,0);
    clock_w = subwin(screen,(h-1)/3,w,(h-1)/3+1,0);
    sender_w = subwin(screen,(h-1)/3-1,w/2,1,0);
    receiver_w = subwin(screen,(h-1)/3-1,w/2,1,w/2);
    text_sender_w =   subwin(screen,h/3-3,w/4-1,2,1+w/4-1);
    text_receiver_w = subwin(screen,h/3-3,w/4-1,2,w/2+1+w/4-1);
    itext_sender_w =   subwin(screen,h/3-5,w/4-2,3,1+w/4);
    itext_receiver_w = subwin(screen,h/3-5,w/4-2,3,w/2+1+w/4);
}
Tela::~Tela()
{
    echo();
    keypad(screen,0);
    delwin(signal_w);
    delwin(clock_w);
    delwin(sender_w);
    delwin(receiver_w);
    delwin(text_receiver_w);
    delwin(text_sender_w);
    delwin(itext_receiver_w);
    delwin(itext_sender_w);
    delwin(screen);
    endwin();
}
void Tela::imprime()
{
    unsigned char tecla = getch();
    if(tecla!=255)
        this->on_key(tecla,this->param);

    erase();
    
    mvwprintw(screen,0,0,"Tempo real: %lu",time(NULL)-this->tempo_inicio);

    mvwaddstr(screen,0,w/2-tamanhos_protocolos[protocolo_atual]/2,nomes_protocolos[protocolo_atual]);

    mvwaddstr(sender_w,1,((w/2-2)/2)/2-7,"##TRANSMISSOR##");

    mvwprintw(sender_w,2,1,"MODO:  %s",nomes_modos[modo_transmissor]);
    mvwprintw(sender_w,(h-1)/3-4,1,"CLOCK:  %s",this->protocolo_atual!=BIPOLAR?nomes_estados[estado_clock]:"N/A");
    mvwprintw(sender_w,(h-1)/3-3,1,"SINAL:  %s",nomes_estados[estado_transmissor]);

    mvwaddnstr(itext_sender_w,0,0,(const char*)this->texto_transmissor,256);

    mvwaddstr(receiver_w,1,((w/2-2)/2)/2-6,"##RECEPTOR##");

    mvwprintw(receiver_w,2,1,"MODO:  %s",nomes_modos[modo_receptor]);
    if(modo_receptor!=2)
    {
        mvwprintw(receiver_w,((h-1)/3-2)/2,1,"BYTE ATUAL: %hhu%hhu%hhu%hhu%hhu%hhu%hhu%hhu",
                    (byte_receptor&(1<<0))>>0,
                    (byte_receptor&(1<<1))>>1,
                    (byte_receptor&(1<<2))>>2,
                    (byte_receptor&(1<<3))>>3,
                    (byte_receptor&(1<<4))>>4,
                    (byte_receptor&(1<<5))>>5,
                    (byte_receptor&(1<<6))>>6,
                    (byte_receptor&(1<<7))>>7);
        mvwaddch(receiver_w,((h-1)/3-2)/2+1,1+12+bit_atual,'T');
    }
    mvwprintw(receiver_w,(h-1)/3-4,1,"CLOCK:    %s",this->protocolo_atual!=BIPOLAR?nomes_estados[estado_clock_receptor]:"N/A");
    mvwprintw(receiver_w,(h-1)/3-3,1,"ENTRADA:  %s",nomes_estados[estado_receptor]);

    mvwaddnstr(itext_receiver_w,0,0,(const char*)this->texto_receptor,TAMANHOTEXTO);
    //mvwprintw(itext_receiver_w,1,0,"%hhu",this->texto_receptor[0]);

    mvwaddstr(screen,(h-1)/3,0,"CLOCK");
    maxC=this->media_clock>maxC?this->media_clock:maxC;
    mvwprintw(screen,(h-1)/3,w-7-6-14,"MAX: %2.3lf   MEDIA: %2.3lf",this->maxC,this->media_clock);

    int i;
    for(i=1;i<w-1;i++)
    {
        int height;
        if(this->protocolo_atual!=BIPOLAR)
            height = ((h-1)/3)*((this->linha_clock.at(i-1)+4)/7);
        else
            height = (h-1)/6;
        mvwaddch(clock_w,height,i,'*');
    }

    mvwaddstr(screen,2*(h-1)/3+1,0,"SINAL");
    maxS=this->media_sinal>maxS?this->media_sinal:maxS;
    mvwprintw(screen,2*(h-1)/3+1,w-7-6-14,"MAX: %2.3lf   MEDIA: %2.3lf",this->maxS,this->media_sinal);

    for(i=1;i<w-1;i++)
    {
        int height = ((h-1)/3)*((this->linha_sinal.at(i-1)+4)/7);
        mvwaddch(signal_w,height,i,'*');
    }

    box(signal_w,0,0);
    box(clock_w,0,0);
    box(receiver_w,0,0);
    box(sender_w,0,0);
    box(text_sender_w,0,0);
    box(text_receiver_w,0,0);
    move(h/3-2,(((w-2)/2)-2));
    refresh();
}

typedef struct{
        Transmissor *t;
        Receptor *r;
        Tela *tela;
        unsigned char sair;
        unsigned char ajustar;
        unsigned char caractere_saida;
    } contexto_evento;

void tecla_apertada(unsigned char tecla,void* param)
{
    contexto_evento *contexto = (contexto_evento*)param;
    switch (tecla)
    {
        case 'q':
            contexto->sair=1;
        break;
        case '1':
            contexto->t->p_atual = BINARIO;
            contexto->r->p_atual = BINARIO;
            goto redefine_tudo;
        case '2':
            contexto->t->p_atual = MANCHESTER;
            contexto->r->p_atual = MANCHESTER;
            goto redefine_tudo;
        case '3':
            contexto->t->p_atual = BIPOLAR;
            contexto->r->p_atual = BIPOLAR;
            goto redefine_tudo;
        case ' ':
            redefine_tudo:
            /*Redefine todos os parâmetros*/
            contexto->t->bit_atual=0;
            contexto->t->fim=0;
            contexto->t->atual=0;
            contexto->t->tempo=0;
            contexto->r->byte=255;
            contexto->r->bit_atual=0;
            contexto->r->clock->media_movel=0;
            contexto->r->sinal->media_movel=0;
            memset(contexto->tela->texto_receptor,0,TAMANHOTEXTO);
            memset(contexto->t->buffer,0,256);
            contexto->caractere_saida=0;
            int i;
            for(i=0;i<contexto->tela->linha_clock.size();i++)
                contexto->tela->linha_clock.at(i)=0;
            for(i=0;i<contexto->tela->linha_sinal.size();i++)
                contexto->tela->linha_sinal.at(i)=0;
        break;
        default:
            contexto->t->putchar(tecla);
        break;
    }
}

void byte_recebido(char c,void* param)
{
    contexto_evento *contexto = (contexto_evento*)param;
    contexto->tela->texto_receptor[contexto->caractere_saida] = c;
    contexto->caractere_saida+=1;
    contexto->caractere_saida%=TAMANHOTEXTO;
}

int main()
{
    /*
    O que passaremos para nossos eventos de tecla
    */
    contexto_evento contexto;
    contexto.sair = 0;

    /*
    Configurando a tela
    */
    Tela *tela = new Tela();
    tela->tempo_inicio=time(NULL);

    /*
    Nossas linhas de transmissão
    */
    std::vector<double> sinal(tela->w-2,0);
    std::vector<double> clock(tela->w-2,0);

    /*
    Configurando a tela
    */
    contexto.tela=tela;
    tela->param = &contexto;
    tela->on_key = tecla_apertada;
    tela->protocolo_atual = BINARIO;
    tela->modo_receptor = 2;
    tela->modo_transmissor = 2;
    tela->estado_receptor = ALTO;
    tela->estado_transmissor = ALTO;
    tela->estado_clock = ALTO;
    tela->estado_clock_receptor = ALTO;
    tela->linha_clock = clock;
    tela->linha_sinal = sinal;

    /*
    Nossa camada física
    */
    contexto.r = new Receptor(.5,2);
    contexto.r->on_char = byte_recebido;
    contexto.r->params = &contexto;
    contexto.caractere_saida=0;
    contexto.t = new Transmissor(.5,2,.8);
    tela->texto_transmissor = contexto.t->buffer;
    contexto.ajustar=1;

    while(not contexto.sair)
    {
        /*
        Mandamos o sinal atual um frame para a frente
        */
        int i;
        for(i=tela->linha_clock.size()-1;i>0;i--)
            tela->linha_clock.at(i)=tela->linha_clock.at(i-1);
        for(i=tela->linha_sinal.size()-1;i>0;i--)
            tela->linha_sinal.at(i)=tela->linha_sinal.at(i-1);

        tela->estado_clock = contexto.t->clock->S;
        tela->estado_transmissor = contexto.t->sinal->S;
        tela->estado_clock_receptor = contexto.r->clock->S;
        tela->estado_receptor = contexto.r->sinal->S;
        tela->byte_receptor = contexto.r->byte;
        tela->bit_atual = contexto.r->bit_atual;
        tela->protocolo_atual = contexto.t->p_atual;
        tela->media_clock = contexto.r->clock->media_movel;
        tela->media_sinal = contexto.r->sinal->media_movel;
        

        if(contexto.ajustar)
        {
            contexto.r->ajustando = 1;
            contexto.t->ajustando = 1;
            tela->modo_transmissor=2;
            tela->modo_receptor=2;
        }
        contexto.r->passatempo(tela->linha_clock.at(tela->linha_clock.size()-1),
                                   tela->linha_sinal.at(tela->linha_sinal.size()-1));
        contexto.t->passatempo();

        tela->linha_clock.at(0)=contexto.t->clock->saida;
        tela->linha_sinal.at(0)=contexto.t->sinal->saida;

        /*Se estiver ajustado*/
        if(contexto.ajustar && contexto.r->delta_clock<0.01 && contexto.r->delta_sinal<0.01)
        {
            contexto.ajustar=0;
            contexto.r->ajustando=0;
            contexto.t->tempo=0;
            contexto.t->ajustando=0;
            tela->modo_transmissor=0;
            tela->modo_receptor=1;
        }

        tela->imprime();
        sleep(1000/60);
    }
    delete tela;
    delete contexto.r;
    delete contexto.t;
    return 0;
}