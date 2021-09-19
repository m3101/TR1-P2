**************************
.TITLE TESTBENCH MOD DEMOD
**************************

%.include ~/Tools/mixedsim/models/yosys/prim_cells_ngspice.mod
.include verilog_spice/mod_demod/mod_demod.mod

VVH high 0 PULSE(-1 1 0 .1 .1 2 0 0)
Vneutral neutral 0 PULSE(-1 1 1 .1 .1 1 2 0)

XMOD high neutral out modulator
XDMOD out demod demodulator

R2 high 0 10K
R3 neutral 0 10K


.control
tran 1ms 4s
set specwindow = hamming
hardcopy synth/vis/modemod.ps v(high) v(neutral)+2 v(out)+4 v(demod)+6
plot v(high) v(neutral)+2 v(out)+4 v(demod)+6
quit
.endc
.end