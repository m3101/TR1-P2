from matplotlib import pyplot as plt
from matplotlib import rcParams
import numpy as np

rcParams['font.family'] = 'monospace'

tempo = np.linspace(0,3,60*3)
amplitude = 3
periodo = 0.3

base = amplitude*np.sin(2*np.pi*tempo/periodo+np.pi/2)
sin = amplitude*np.sin(2*np.pi*tempo/periodo)
sinal = amplitude*np.sin(2*np.pi*tempo/periodo)
third = len(tempo)//3
sinal[third:2*third] = amplitude*np.sin(2*np.pi*tempo[third:2*third]/periodo+np.pi/2)
sinal[2*third:] = amplitude*np.sin(2*np.pi*tempo[2*third:]/periodo-np.pi/2)

multi = base*sinal

alisamento = .90
movavg = np.zeros((len(tempo)))
for i in range(1,len(tempo)):
    movavg[i]=movavg[i-1]*alisamento+(1-alisamento)*multi[i]

fig, axs = plt.subplots(3)
axs[0].set_title("Base")
axs[0].plot(tempo,base,'-k',linewidth=1)
axs[1].set_title("Modulado")
axs[1].plot(tempo,sinal,'-k',linewidth=1)
axs[1].set_ylabel("Sinal (Volts)")
axs[2].set_title("Multiplicado")
axs[2].plot(tempo,multi,'-k',linewidth=1)
axs[2].set_xlabel("Tempo (segundos)")
plt.tight_layout()
plt.figure()
plt.plot(tempo,movavg,'-k',linewidth=1)
plt.xlabel("Tempo (segundos)")
plt.ylabel("Média Móvel (Volts)")
fig,axs = plt.subplots(4)
axs[0].set_title("Original")
axs[0].plot(tempo,sin,'-k',linewidth=1)
axs[0].axvline(0.3+.3/4,-2.5,2.5,linestyle='-',color='k',linewidth=1)
axs[2].set_title("Frequência aumentada")
axs[2].plot(tempo,amplitude*np.sin(3*np.pi*tempo/periodo),'-k',linewidth=1)
axs[1].set_title("Fase aumentada")
axs[1].axvline(0.3,-2.5,2.5,linestyle='-',color='k',linewidth=1)
axs[1].plot(tempo,base,'-k',linewidth=1)
axs[3].set_title("Amplitude diminuida")
axs[3].set_ylim(-2.5,2.5)
axs[3].plot(tempo,sin*0.1,'-k',linewidth=1)
plt.tight_layout()
plt.show()