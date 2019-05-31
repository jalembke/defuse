# Set formatting
set output out_file         # Set the output path

# Set the font to something pleasing
set term svg fname "Times,19" size 225, 250
unset key                   # No key
set grid                    # Turn the grid on

#set title gtitle offset 0,-0.7
set style fill solid 0.25 border
set boxwidth 0.40

# Create the plot
set format y "%.0f"
set ylabel "Write Speed (MB/sec)" offset 2.7
set yrange [0:*]
set ytics 10,10 offset graph 0.07
set xtics offset 0,graph 0.07

set lmargin at screen 0.25
set rmargin at screen 0.98
set tmargin at screen 0.97
set bmargin at screen 0.13

set style line 1 lc 12
set style line 2 lc 5

plot  dat_file every ::0 using 1:($3):xtic(2) with boxes ls 1 fs pattern 1, \
      dat_file every ::1 using 1:($3):xtic(2) with boxes ls 2 fs pattern 2;
