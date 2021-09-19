from matplotlib import pyplot as plt
from matplotlib import rcParams
import numpy as np

rcParams['font.family'] = 'monospace'

def generate(graficos:dict,pulsesize:int=5,vlines=[],bits=[],bitrate=0.2):
    scale = 1/(6*pulsesize*bitrate)
    fig, axs = plt.subplots(len(graficos))
    time = np.arange(max([len(graficos[k])*5 for k in graficos])).astype(float)
    time *=scale
    vlines = [i*pulsesize*scale for i,v in enumerate(vlines) if v==1]
    if len(graficos.keys())==1:
        axs = [axs]
    for i,g in enumerate(graficos):
        axs[i].set_title(g)
        y = np.zeros(time.shape)
        for n in range(len(graficos[g])):
            y[n*pulsesize:(n+1)*pulsesize] = graficos[g][n]
        axs[i].vlines(vlines,y.min()*1.2,y.max()*1.2,colors='#777777',linestyles='dashed',linewidth=1)
        axs[i].set_xlabel("Tempo (s)")
        axs[i].set_ylabel("Sinal (V)")
        if i>0 or len(axs)==1:
            for j,x in enumerate(vlines):
                axs[i].text(x,0.1,str(bits[j]),ha="center",va="center",bbox=dict(boxstyle="square",
                   ec=(0,0,0),
                   fc=(1.,1.,1.),
                   ))
        plt.tight_layout()
        axs[i].plot(time,y,'-k',linewidth=1)
def show():
    plt.show()