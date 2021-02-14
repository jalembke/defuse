# Set formatting
set output out_file         # Set the output path

# Set the font to something pleasing
set term svg fname "Times,19" size 220, 220
unset key                   # No key
set grid                    # Turn the grid on

set style data histogram
set style fill pattern border
#set style histogram cluster gap 0.5
set boxwidth 0.5 # 0.8

#set title gtitle offset 0,-0.7
#set ylabel "Write Speed (MBps)" offset 3.5

unset xtics
set format y yformat
set ytics yticSpread
set ytics border in nomirror scale 0.4
set ytics offset graph 0.07

set yrange [0:*]
set xrange [-0.7:2.7]

#set lmargin at screen 0.25
#set rmargin at screen 0.98
#set tmargin at screen 0.97
#set bmargin at screen 0.02

set style line 1 lt 6
set style line 2 lt 1
set style line 3 lt 3

#plot  dat_file every ::0::0 using 1:3:xtic(2) with boxes ls 1 fs pattern 6, \
#      dat_file every ::1::1 using 1:3:xtic(2) with boxes ls 2 fs pattern 1, \
# 	   dat_file every ::2::2 using 1:3:xtic(2) with boxes ls 3 fs pattern 4;
plot  dat_file every ::0::0 using 1:3 with boxes ls 1 fs pattern 6, \
      dat_file every ::1::1 using 1:3 with boxes ls 2 fs pattern 1, \
	  dat_file every ::2::2 using 1:3 with boxes ls 3 fs pattern 4;
