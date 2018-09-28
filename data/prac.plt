set output out_file         # Set the output path
set term svg fname "Times,19" size 300, 350
set grid                    # Turn the grid on

unset key

#set title gtitle offset 0,-0.8 font "Times,22"
#set xrange [-0.7:*]
set yrange [0:*]
set style data histogram
set style histogram cluster gap 0.5
#set style fill solid 0.25 border
set style fill pattern border
set boxwidth 0.8
set xtic scale 0
#set ytics 0.5

#set xtics offset 1.3,graph 0.4
unset xtics

set tmargin at screen 0.97
set rmargin at screen 0.97
set lmargin at screen 0.24
set bmargin at screen 0.04

set ylabel pracTitle offset 2.8 font "Times,22"

# 2, 3, 4, 5 are the indexes of the columns; 'fc' stands for 'fillcolor'
plot dat_file using 2:xtic(1) ti col lc 6 fs pattern 6, \
           '' using 3 ti col lc 1 fs pattern 1, \
           '' using 4 ti col lc 2 fs pattern 2, \
           '' using 5 ti col lc 3 fs pattern 4, \
           '' using 6 ti col lc 4 fs pattern 5
