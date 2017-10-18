# Set formatting
set output out_file         # Set the output path

# Set the font to something pleasing
set term svg fname "Times,19" size 400, 300
unset key                   # No key
set grid                    # Turn the grid on

set style fill solid 0.25 border
set boxwidth 0.40

# Create the plot
set format y "%.0f"
set ylabel "Writes (MB/sec)"
set yrange [0:*]
set ytics 4

set style line 1 lc 12
set style line 2 lc 5

plot  dat_file every ::0 using 1:($3/1000000):xtic(2) with boxes ls 1, \
      dat_file every ::1 using 1:($3/1000000):xtic(2) with boxes ls 2;
