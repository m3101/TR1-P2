main:
	@if [ -d build ]; then\
		rm -rf build;\
	fi
	@mkdir build
	cd src;\
	g++ -o ../build/sim Simulador.cpp CamadaFisica.cpp -lm -lncurses
testemod:
	@if [ -d build ]; then\
		rm -rf build;\
	fi
	@mkdir build
	cd src;\
	g++ -o ../build/sim Teste_modulacao.cpp CamadaFisica.cpp -lm
testevisual:
	@if [ -d build ]; then\
		rm -rf build;\
	fi
	@mkdir build
	cd src;\
	g++ -o ../build/sim Teste_visual.cpp CamadaFisica.cpp -lm -lncurses
physical:
	@if [ -d synth ]; then\
		rm -rf synth;\
	fi
	@mkdir synth
	@mkdir synth/vis
	ngspice verilog_spice/mod_demod/testbench_mod_demod.mod
	ngspice verilog_spice/mod_demod/testbench_spectral_a.mod