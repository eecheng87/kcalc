reset
set title 'performance'
set xlabel 'exp. time'
set ylabel 'time (ns)'
set key right top
set term png enhanced font 'Verdana,10'
set output 'perf1.png'
plot [:][:] \
'perf1.out' using 1:3 with linespoints linewidth 2 title "MathEx fix",\
'' using 1:2 with linespoints linewidth 2 title "rt fix",\
