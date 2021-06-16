# Set formatting
set output out_file         # Set the output path

# Set the font to something pleasing
set term svg fname "Times,19" size 220, 220
unset key                   # No key
set grid                    # Turn the grid on

set errorbars fullwidth
set style data histogram
set style histogram cluster gap 0.5
set style histogram errorbars gap 2 lw 1
set style fill pattern border lt -1
#set style histogram cluster gap 0.5
set boxwidth 0.5 # 0.8

#set title gtitle offset 0,-0.7
#set ylabel "Context Switches" offset 2.3

unset xtics
set logscale y
set format y "10^%L" # "10^%T"
set ytics add ('1' 1)
set ytics border in nomirror scale 0.4
set ytics offset graph 0.07

set yrange [1:*] # [1:150000]
set xrange [-0.325:0.325]
#set xrange [-0.7:2.7]

#set lmargin at screen 0.20
#set rmargin at screen 0.98
#set tmargin at screen 0.93
#set bmargin at screen 0.05

set style line 1 lt 6
set style line 2 lt 1
set style line 3 lt 3

#plot  dat_file every ::0::0 using 1:3:xtic(2) with boxes ls 1 fs pattern 6, \
#      dat_file every ::1::1 using 1:3:xtic(2) with boxes ls 2 fs pattern 1, \
#      dat_file every ::2::2 using 1:3:xtic(2) with boxes ls 3 fs pattern 4;
#plot  dat_file every ::0::0 using 1:3 with boxes ls 1 fs pattern 6, \
#      dat_file every ::1::1 using 1:3 with boxes ls 2 fs pattern 1, \
#      dat_file every ::2::2 using 1:3 with boxes ls 3 fs pattern 4;

plot dat_file using 2:3:4:xtic(1) ti col lc 6 fs pattern 6, \
           '' using 5:6:7 ti col lc 1 fs pattern 1, \
           '' using 8:9:10 ti col lc 3 fs pattern 4
