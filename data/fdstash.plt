# Set formatting
set output out_file         # Set the output path

# Set the font to something pleasing
set term svg enhanced fname "Times,18" size 500, 250
set key at 2700,22 spacing 0.75   # Place the key in an appropriate place
set key font "Times,18"
set autoscale xfix          # Set axes automatically but fix X range to min and max value
unset logscale              # Clear log scale
set grid                    # Turn the grid on
set format y "%.0f"
set ytics offset graph 0.02,0
set xtics 500, 500 offset 0,graph 0.07

set yrange [0:*]
set xrange [1:4100]

set tmargin at screen 0.95
set rmargin at screen 0.95
set lmargin at screen 0.12
set bmargin at screen 0.20

# Create the plot
set xlabel "Inherited File Descriptors" offset 0,1.3
set ylabel "Overhead (μsec)" offset 2.0,0

f(x) = a*x + b
fit f(x) dat_file u 1:2 via a, b
title_f(a,b) = sprintf('f(x) = %.2fx + %.2f', a, b)

plot dat_file using 1:2 with linespoints pt 7 ps 0.5 lw 3 lc 1 title "Experimental Results", \
     dat_file with errorbars ps 0 lc 1 lw 1 notitle, \
     f(x) lw 3 title "Linear Regression"
