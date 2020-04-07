reset
set title 'performance'
set xlabel 'exp. time'
set ylabel 'time (ns)'
set key right top
set term png enhanced font 'Verdana,10'
set output 'perf2.png'
plot [:][:] \
'mathexmul.out' using 1:2 with linespoints linewidth 2 title "MathEx mul",\
'rtmul.out' using 1:2 with linespoints linewidth 2 title "rt mul",\
