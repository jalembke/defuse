set output out_file         # Set the output path
set term svg fname "Times,19" size 200, 250
set grid                    # Turn the grid on

unset key

#set title gtitle offset 0,-0.7 font "Times,22"
set xrange [-0.7:*]
set yrange [0:*]
set style data histogram
set style histogram cluster gap 0.5
#set style fill solid 0.25 border
set style fill pattern border
set boxwidth 0.8
set xtic scale 0

#set xtics offset 1.3,graph 0.04
unset xtics

set tmargin at screen 0.97
set rmargin at screen 0.97
set lmargin at screen 0.34
set bmargin at screen 0.04

#set ylabel "Runtime (sec)" offset 3.0 font "Times,22"
set ylabel "Read Bandwidth (MB/s)" offset 2.4 font "Times,22"
set ytics offset 0.3

# 2, 3, 4, 5 are the indexes of the columns; 'fc' stands for 'fillcolor'
plot dat_file using 2:xtic(1) ti col fs pattern 1, \
           '' using 3 ti col fs pattern 2, \
           '' using 4 ti col fs pattern 4, \
           '' using 5 ti col fs pattern 5
