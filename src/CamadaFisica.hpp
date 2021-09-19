#ifndef CAMADAFISICAH
#define CAMADAFISICAH

/*
* MODULAÇÃO
*/

/*
Gera uma amostra de onda senoidal para um certo tempo,
fase e período
*/
double onda_base(double tempo,double periodo, double fase);

/*
Estados possíveis para um (de-)modulador
*/
enum estado {BAIXO,NEUTRO,ALTO};


/*
Uma função que pode ser passada como evento para um
demodulador
*/
typedef void (*callback_demodulador)(void *self,void *param);
/*
Um demodulador de sinais analógicos.
Usa modulação de fase binária.
*/
class Demodulador
{
private:
    double amplitude;
    double referencia;
    /*Taxa de alisamento da média móvel*/
    double alisamento;
    /*Distância máxima do estado atual para causar uma mudança*/
    double delta_max;
    /*
    Atualiza a média móvel com uma nova amostra
    */
    void _media_movel(double amostra);
    /*
    Multiplica o sinal pela onda de referência
    */
    double _mult_amostra(double amostra);
    /*
    Faz a transição de estados
    * trigger: determina se a atualização enviará eventos de mudança
    */
    void _transicao(unsigned char trigger);
public:

    /*
    Um demodulador.
    * * fase: fase de referência (pode ser ajustada automaticamente depois)
    * * periodo: período da onda de referência
    * * amplitude: amplitude da onda de referência
    * * referencia: valor de referência para o "ALTO" na média móvel
    * * delta_max: distância máxima do estado atual para causar mudança
    * * alisamento: taxa de alisamento para a média móvel (afeta o tempo de resposta do demodulador)
    */
    Demodulador(double fase,
                double periodo,
                double amplitude,
                double referencia,
                double delta_max,
                double alisamento);
    ~Demodulador();

    /*
    Ajusta a fase assumindo que a entrada está num valor "ALTO"
    Retorna a distância do valor atual à referência
    */
    double ajusta_fase(double amostra);

    double media_movel,tempo,fase;
    /*
    Intervalo básico de tempo da simulação
    */
    double deltat;

    /*
    Período da onda de referência
    */
    double periodo;

    /*
    Estado atual do demodulador.
    */
    estado S;
    /*
    Processa uma amostra.
    */
    void recebe_amostra(double amostra);
    /*
    Chamada quando passamos de um estado menor para maior
    */
    callback_demodulador posedge;
    /*
    Chamada quando passamos de um estado maior para menor
    */
    callback_demodulador negedge;
    /*
    Parâmetro a passar para o callback
    */
    void* param;
};

/*
Um modulador de sinais analógicos.
Usa modulação de fase binária.
*/
class Modulador
{
private:
    double amplitude;
public:
    /*
    Um modulador.
    * * periodo: período da onda de referência
    * * amplitude: amplitude da onda de referência
    */
    Modulador(double periodo,
              double amplitude);
    ~Modulador();

    double tempo;

    /*
    Intervalo básico de tempo da simulação
    */
    double deltat;

    /*
    Período da onda de referência
    */
    double periodo;

    /*
    Estado atual do modulador.
    */
    estado S;
    /*

    /*
    A saída atual do modulador
    */
    double saida;
    
    /*
    Avança o tempo em um intervalo.
    */
    void passatempo();
};

/*
* TRANSMISSÃO
*/

enum protocolo {BINARIO,MANCHESTER,BIPOLAR};

class Transmissor
{
private:
public:
    Modulador* clock;
    Modulador* sinal;
    /*
    Um buffer de caracteres a serem enviados
    */
    unsigned char buffer[256];
    unsigned char atual;
    unsigned char bit_atual;
    /*
    1 byte depois do fim da lista
    */
    unsigned char fim;

    /*
    Determinam a taxa de envio de bits
    */
    double periodo_bit,tempo;

    /*
    O protocolo a usar para a transmissão.
    */
    protocolo p_atual;

    /*
    O menor intervalo de tempo da simulação
    */
    double deltat;

    /*
    Se verdadeiro, não transmitirá dados.
    Coloca ambos os sinais em "ALTO" para ajuste
    */
    unsigned char ajustando;

    /*
    Passa um quadro de tempo
    */
    void passatempo();

    /*
    Põe um caractere no buffer para transmissão.
    */
    void putchar(char c);

    /*
    Um transmissor simples de dados
    * * bitrate: taxa de bits a enviar por segundo
    */
    Transmissor(double periodo,double amplitude,double bitrate);
    ~Transmissor();
};

/*
Função aceitável como callback para a recepção de um caractere
*/
typedef void (*callback_recebe_char)(char c,void* params);

void _receptor_on_posedge_sinal(Demodulador *self,void* receptor);
void _receptor_on_negedge_sinal(Demodulador *self,void* receptor);
void _receptor_on_posedge_clock(Demodulador *self,void* receptor);
void _receptor_on_negedge_clock(Demodulador *self,void* receptor);

class Receptor
{
private:
public:
    Demodulador *clock;
    Demodulador *sinal;

    /*
    Distâncias atuais da referência ALTA
    para o valor real
    */
    double delta_clock;
    double delta_sinal;

    unsigned char bit_atual;
    /*
    Buffer de 8 bits
    */
    unsigned char byte;

    /*
    O protocolo a usar para a transmissão.
    */
    protocolo p_atual;

    /*
    O menor intervalo de tempo da simulação
    */
    double deltat;

    /*
    Se verdadeiro, não receberá dados.
    Somente ajustará a fase de referência assumindo
    que a entrada está ALTA
    */
    unsigned char ajustando;

    /*
    Passa um quadro de tempo
    Recebe os sinais atuais de clock e sinal
    */
    void passatempo(double clock, double sinal);

    /*
    Parâmetro a passar para o callback
    */
    void* params;
    /*
    Callback chamado quando um caractere (byte) é recebido
    */
    callback_recebe_char on_char;
    
    /*
    Um receptor de dados
    */
    Receptor(double periodo, double amplitude);
    ~Receptor();
};

#endif