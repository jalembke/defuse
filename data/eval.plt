set output out_file         # Set the output path
set term svg fname "Times,25" size 600, 300
set grid ytics              # Turn the grid on for yaxis

unset key

#set title gtitle offset 0,-0.7
set auto x
set yrange [0:100]
set style data histogram
set style histogram cluster gap 1
#set style fill solid 0.25 border
set style fill pattern border
set boxwidth 0.8
set xtic scale 0

set xtics offset 0,graph 0.10
set ytics offset graph 0.02
set ytics border in nomirror scale 0.4

set tmargin at screen 0.97
set rmargin at screen 0.98
set lmargin at screen 0.18
set bmargin at screen 0.08

set ylabel "Throughput\n(% of Direct Mount)" offset 3.7

# 2, 3, 4, 5 are the indexes of the columns; 'fc' stands for 'fillcolor'
plot dat_file using 2:xtic(1) ti col fs pattern 1, \
           '' using 3 ti col fs pattern 2, \
           '' using 4 ti col fs pattern 4, \
           '' using 5 ti col fs pattern 5
