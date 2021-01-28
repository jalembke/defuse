# Set formatting
set output out_file         # Set the output path

# Set the font to something pleasing
set term svg fname "Times,19" size 350, 250
unset key                   # No key
set grid                    # Turn the grid on

set style data histogram
set style fill pattern border
#set style histogram cluster gap 0.5
set boxwidth 0.5 # 0.8

#set title gtitle offset 0,-0.7
#set ylabel "Write Speed (MBps)" offset 3.5

unset xtics
#set format y yformat
#set ytics yticSpread
#set xtics offset 0, graph 0.10
set ytics border in nomirror scale 0.4
set ytics offset graph 0.03

set yrange [0:*]
set xrange [-0.7:3.7]

#set tmargin at screen 0.96
#set rmargin at screen 0.98
#set lmargin at screen 0.12
#set bmargin at screen 0.12

#set ylabel "Runtime (s)"

# 2, 3, 4, 5 are the indexes of the columns; 'fc' stands for 'fillcolor'
plot  dat_file every ::0::0 using 1:3:4:xtic(2) with boxes fs pattern 6 lc 6, \
      dat_file every ::0::0 using 1:3:4:5 with errorbars pt 0 lc 8, \
	  dat_file every ::1::1 using 1:3:4:xtic(2) with boxes fs pattern 2 lc 2, \
      dat_file every ::1::1 using 1:3:4:5 with errorbars pt 0 lc 8, \
	  dat_file every ::2::2 using 1:3:4:xtic(2) with boxes fs pattern 4 lc 3, \
      dat_file every ::2::2 using 1:3:4:5 with errorbars pt 0 lc 8;
