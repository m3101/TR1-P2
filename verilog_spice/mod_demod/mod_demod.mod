************************
*MODULADOR E DEMODULADOR
.TITLE MODULADOR E DEMODULADOR
* Descreve os circuitos de modulação e demodulação
************************

.model mult mult
.model mult_digital mult(in_offset=[0,1],out_gain=0.5)
.model add summer

.SUBCKT inverter value out
    BVCC vcc 0 v=-1
    AOUT [vcc,value] out mult
.ends inverter

.SUBCKT modulator high neutral out
    *Senóides de referência
    VHIGH highp 0 SIN(0 1 2 0 0 0)
    VNEUTRAL neutralp 0 SIN(0 1 2 0 0 90)
    VLOW lowp 0 SIN(0 1 2 0 0 180)

    * out = neutralp*neutral+((highp*high)+(lowp*not_high))*not_neutral

    XNH high not_high inverter
    XNN neutral not_neutral inverter

    AFH [highp,high] hpf mult_digital
    AFL [lowp,not_high] lpf mult_digital
    ASNN [hpf,lpf] nn add

    AFN [neutralp,neutral] npf mult_digital
    AFNN[nn,not_neutral] nnf mult_digital

    AOUT[nnf,npf] out add

.ENDS modulator

.SUBCKT demodulator in out
    *Senóide de referência
    VHIGH highp 0 SIN(0 1 2 0 0 0)

    AM [highp,in] mult mult

    R1 mult out 1.5
    C3 out 0 100m

.ends demodulator