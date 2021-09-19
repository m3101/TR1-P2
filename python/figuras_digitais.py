import gerador_figuras_digitais

#Codificacoes
clock =  [ 0, 0,-1,-1,-1, 1, 1, 1,-1,-1,-1, 1, 1, 1,-1,-1,-1, 1, 1, 1,-1,-1,-1, 1, 1, 1, 0, 0]
vlines = [ 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0]
bits = [1,0,1,1]
binaria= [ 0, 0, 1, 1, 1, 1, 1, 1,-1,-1,-1,-1,-1,-1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0]
manches= [ 0, 0, 1, 1, 1,-1,-1,-1,-1,-1,-1, 1, 1, 1, 1, 1, 1,-1,-1,-1, 1, 1, 1,-1,-1,-1, 0, 0]
manchesa=[ 0, 0, 0, 0, 1, 1, 1,-1,-1,-1,-1,-1,-1, 1, 1, 1, 1, 1, 1,-1,-1,-1, 1, 1, 1,-1,-1,-1, 0, 0]
bipolar =[ 0, 0, 1, 1, 0,-1,-1, 0, 1, 1, 0, 1, 1, 0, 0]
vlines_b=[ 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0]
gerador_figuras_digitais.generate({
    'Clock':clock,
    'Codificação Binária':binaria
},5,vlines,bits)
gerador_figuras_digitais.generate({
    'Clock':clock,
    'Codificação Manchester':manches
},5,vlines,bits)
gerador_figuras_digitais.generate({
    'Clock':clock,
    'Codificação Manchester':manches,
    'Codificação Manchester Atrasada':manchesa
},5,vlines,bits)
gerador_figuras_digitais.generate({
    'Codificação Bipolar':bipolar
},5,vlines_b,bits)
gerador_figuras_digitais.show()