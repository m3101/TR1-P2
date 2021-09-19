**************************
.TITLE SPECTRAL ANALYSYS
**************************

%.include ~/Tools/mixedsim/models/yosys/prim_cells_ngspice.mod
.include verilog_spice/mod_demod/mod_demod.mod

VVH high 0 PULSE(-1 1 0 .1 .1 1 4 0)

XMOD high neutral out modulator
XDMOD out demod demodulator

R2 high 0 10K
R3 neutral 0 10K


.control
tran 1ms 20s
set specwindow = gaussian
fft v(out) v(high)
hardcopy synth/vis/fftsig.ps mag(v(out)) xlimit 0 5
hardcopy synth/vis/fftsquare.ps mag(v(high))  xlimit 0 5
plot mag(v(out)) xlimit 0 5
plot mag(v(high)) xlimit 0 5
quit
.endc
.end