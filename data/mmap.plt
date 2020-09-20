set output out_file         # Set the output path
set term svg fname "Times,25" size 550, 250
set grid ytics              # Turn the grid on for yaxis

unset key

#set auto x
set style fill pattern border
set xrange [-1.5:4.5]
set xtic scale 0
set yrange [0:*]

set boxwidth 0.5 relative

set xtics offset 0,graph 0.12
set ytics offset graph 0.02
set ytics border in nomirror scale 0.4

set tmargin at screen 0.96
set rmargin at screen 0.98
set lmargin at screen 0.12
set bmargin at screen 0.12

set ylabel "MMAP Runtime (s)" offset 3.9

# 2, 3, 4, 5 are the indexes of the columns; 'fc' stands for 'fillcolor'
plot  dat_file every ::0::0 using 1:3:4:xtic(2) with boxes fs pattern 6 lc 6, \
      dat_file every ::0::0 using 1:3:4:5 with errorbars pt 0 lc 8, \
	  dat_file every ::1::1 using 1:3:4:xtic(2) with boxes fs pattern 2 lc 2, \
      dat_file every ::1::1 using 1:3:4:5 with errorbars pt 0 lc 8, \
	  dat_file every ::2::2 using 1:3:4:xtic(2) with boxes fs pattern 4 lc 3, \
      dat_file every ::2::2 using 1:3:4:5 with errorbars pt 0 lc 8;
