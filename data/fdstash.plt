# Set formatting
set output out_file         # Set the output path

# Set the font to something pleasing
set term svg fname "Times,19" size 400, 250

set key at 3200,24          # Place the key in an appropriate place
set key font "Times,18"
set key samplen 1
set key spacing 0.75

set autoscale xfix          # Set axes automatically but fix X range to min and max value
unset logscale              # Clear log scale
set grid                    # Turn the grid on

set format y "%.0f"
set xtics 0, 500 #offset 0, graph 0.07
set xtics rotate by 30 right offset 0.5,0.1
set ytics offset graph 0.02,-0.01
set xtics border in nomirror scale 0.4
set ytics border in nomirror scale 0.4

set yrange [0:*]
set xrange [-100:4100]

#set tmargin at screen 0.95
#set rmargin at screen 0.95
#set lmargin at screen 0.12
#set bmargin at screen 0.20

# Create the plot
set xlabel "Inherited File Descriptors" offset 0,0.9
set ylabel "Overhead (μs)" offset 1.5,0

f(x) = a*x + b
fit f(x) dat_file u 1:2 via a, b
title_f(a,b) = sprintf('f(x) = %.2fx + %.2f', a, b)


plot dat_file using 1:2 with linespoints pt 7 ps 0.5 lw 3 lc 1 dt 2 title "Experimental Results", \
     f(x) lw 3 lc 3 title "Linear Regression", \
     dat_file with errorbars ps 0 lc 1 lw 1 notitle, \
