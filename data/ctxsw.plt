set output out_file         # Set the output path
set term svg fname "Times,19" size 300, 250
set grid ytics              # Turn the grid on for yaxis

unset key

#set title gtitle offset 0,-0.7
set errorbars fullwidth
set style data histogram
set style histogram cluster gap 1.0
set style histogram errorbars gap 2 lw 1
#set style fill solid 0.25 border
set style fill pattern border lt -1
set boxwidth 0.8

set xrange [-0.55:1.55]
#set yrange [0:100]

set xtic scale 0
set xtics offset 0,graph 0.10

set logscale y
set format y "10^{%L}"
set ytics add ('1' 1)
set ytics border in nomirror scale 0.6
set ytics offset graph 0.02, graph -0.05

#set tmargin at screen 0.97
#set rmargin at screen 0.98
#set lmargin at screen 0.20
#set bmargin at screen 0.08

#set ylabel "I/O Context Switches" offset 4.5

# 2, 3, 4, 5 are the indexes of the columns; 'fc' stands for 'fillcolor'
plot dat_file using 2:3:4:xtic(1) ti col lc 6 fs pattern 6, \
           '' using 5:6:7 ti col lc 1 fs pattern 1, \
           '' using 8:9:10 ti col lc 7 fs pattern 7, \
           '' using 11:12:13 ti col lc 2 fs pattern 2, \
           '' using 14:15:16 ti col lc 3 fs pattern 4, \
           '' using 17:18:19 ti col lc 4 fs pattern 5,
