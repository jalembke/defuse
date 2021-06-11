set output out_file         # Set the output path
set term svg fname "Times,19" size 230, 250
set grid ytics              # Turn the grid on for yaxis

unset key

#set title gtitle offset 0,-0.7 font "Times,22"
set style data histogram
set style histogram cluster gap 0.5
set style histogram errorbars gap 2 lw 1
#set style fill solid 0.25 border
set style fill pattern border lt -1
set boxwidth 0.8

set xrange [-0.4:0.4]
set yrange [0:*]

#set xtics offset 1.3,graph 0.04
set xtic scale 0
unset xtics
set ytics offset 0.3
set ytics border in nomirror scale 0.4
set format y yformat

#set tmargin at screen 0.97
#set rmargin at screen 0.97
#set lmargin at screen 0.34
#set bmargin at screen 0.04

#set ylabel "Runtime (sec)" offset 3.0 font "Times,22"
#set ylabel "Read Bandwidth (MB/s)" offset 2.4 font "Times,22"

# 2, 3, 4, 5 are the indexes of the columns; 'fc' stands for 'fillcolor'
plot dat_file using ($2/toMB):($3/toMB):($4/toMB):xtic(1) ti col fs pattern 1, \
           '' using ($5/toMB):($6/toMB):($7/toMB) ti col fs pattern 2, \
           '' using ($8/toMB):($9/toMB):($10/toMB) ti col fs pattern 4, \
           '' using ($11/toMB):($12/toMB):($13/toMB) ti col fs pattern 5
