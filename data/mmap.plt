# Horizontal histogram hack, thanks to:
# http://www.phyast.pitt.edu/~zov1/gnuplot/html/histogram.html
reset

set output out_file         # Set the output path
set term svg fname "Times,19" size 350, 350
set grid y2tics             # Turn the grid on for yaxis

unset key

#set auto x
set style histogram gap 0.5
set style fill pattern border
set boxwidth 0.8

set xtic scale 0
set xtics rotate by 90 offset 0, -1.5
unset ytics
set y2tics rotate by 90 offset -1,-0.45
set y2tics border in nomirror scale 0.4

set xrange [-1:4]
set yrange [0:0.6]
set y2range [0:0.6]

#set tmargin at screen 0.96
#set rmargin at screen 0.98
#set lmargin at screen 0.12
#set bmargin at screen 0.12

set y2label "MMAP Runtime (s)" offset -4.5

# 2, 3, 4, 5 are the indexes of the columns; 'fc' stands for 'fillcolor'
plot  dat_file every ::0::0 using 1:3:4:xtic(2) with boxes fs pattern 6 lc 6, \
      dat_file every ::0::0 using 1:3:4:5 with errorbars pt 0 lc 8, \
	  dat_file every ::1::1 using 1:3:4:xtic(2) with boxes fs pattern 2 lc 2, \
      dat_file every ::1::1 using 1:3:4:5 with errorbars pt 0 lc 8, \
	  dat_file every ::2::2 using 1:3:4:xtic(2) with boxes fs pattern 4 lc 3, \
      dat_file every ::2::2 using 1:3:4:5 with errorbars pt 0 lc 8;
