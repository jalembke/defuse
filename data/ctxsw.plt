set output out_file         # Set the output path
set term svg fname "Times,25" size 600, 400
set grid                    # Turn the grid on

unset key

set title gtitle offset 0,-0.7
set xrange [-0.7:1.7]
#set yrange [0:100]
set style data histogram
set style histogram cluster gap 1.0
set style fill solid 0.25 border
set boxwidth 0.8
set xtic scale 0

set logscale y
set xtics offset 0,graph 0.07
set ytics offset graph 0.02

set tmargin at screen 0.87
set rmargin at screen 0.98
set lmargin at screen 0.20
set bmargin at screen 0.08

set ylabel "I/O Context Switches" offset 4.5

# 2, 3, 4, 5 are the indexes of the columns; 'fc' stands for 'fillcolor'
plot dat_file using 2:xtic(1) ti col fc 6, \
           '' using 3 ti col fc 1, \
           '' using 4 ti col fc 2, \
           '' using 5 ti col fc 3, \
           '' using 6 ti col fc 4, \
           '' using 7 ti col fc 5, \
           '' using 8 ti col fc 8
