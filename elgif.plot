set palette grey
set term gif animate
set output 'Red.gif'
set xra[-0.5:121.5]
set yra[-0.5:121.5]
do for[ii=0:100] {plot 'resultados.dat' i ii matrix with image}
