reset
set title 'precision visualization'
set key right top
set term png enhanced font 'Verdana,10'
set output 'fix1.png'
plot [0:3][13:16] \
'rt_fix.out' using 2:4 with points linewidth 2 title "MathEx fix",\
'rt_fix.out' using 1:3 with points linewidth 2 title "rt fix",\
