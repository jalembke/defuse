set output out_file         # Set the output path
set term svg fname "Times,25" size 1400, 400
set grid                    # Turn the grid on

unset key

#set title gtitle offset 0,-0.7
set auto x
set style data histogram
set style histogram cluster gap 1
#set style fill solid 0.25 border
set style fill pattern border
set boxwidth 0.8
set xtic scale 0

set xtics rotate offset 0,graph 0.04
set ytics offset graph 0.01

set tmargin at screen 0.97
set rmargin at screen 0.99
set lmargin at screen 0.07
set bmargin at screen 0.25

set ylabel "Runtime (% of Direct Mount)" offset 3.8

# 2, 3, 4, 5 are the indexes of the columns; 'fc' stands for 'fillcolor'
plot dat_file using 2:xtic(1) ti col fs pattern 2 lc 2, \
           '' using 3 ti col fs pattern 4 lc 3
