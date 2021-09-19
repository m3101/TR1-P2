# Simulador da camada física de transmissão

Amélia O. F. da S. - 190037971

---

## Simulador em C++

### Instruções de compilação

As regras de construção `main`(simulador, chamada por `make` ou `make main`) e `testevisual` (`make testevisual`) dependem da presença da biblioteca `ncurses`.

`make` compila o código principal.

`make testemod` compila um teste básico dos moduladores/demoduladores;

`make testevisual` compila um teste básico da interface e da transmissão.

### Instruções de uso
Na interface principal, os seguintes comandos/teclas são reconhecidos:


| Tecla | Ação                               |
|-------|------------------------------------|
| 1     | Ativa o protocolo BINÁRIO          |
| 2     | Ativa o protocolo MANCHESTER       |
| 3     | Ativa o protocolo BIPOLAR          |
| Espaço| Redefine a simulação               |
| Outras| Adiciona a tecla à transmissão     |

---

## Simulador a nível de circuito

A simulação da modulação a nível de circuito foi realizada utilizando a ferramenta de código aberto `NGSPICE`.

O comando `make physical` gera os gráficos utilizados no Apêndice assumindo que o simulador esteja corretamente configurado (o simulador exige a habilitação de simulações de sinal misto mediadas por C, o `XSPICE`, que não é habilitado em algumas distribuições. Compile o programa no seu sistema habilitando esta ferramenta).