set output out_file         # Set the output path
set term svg fname "Times,19" size 600, 400
set grid                    # Turn the grid on

unset key

set title gtitle
set auto x
set yrange [0:*]
set style data histogram
set style histogram cluster gap 1
set style fill solid 0.25 border
set boxwidth 0.8
set xtic scale 0

set ylabel "Performnace (% of Direct Mount)"

# 2, 3, 4, 5 are the indexes of the columns; 'fc' stands for 'fillcolor'
plot dat_file using 2:xtic(1) ti col, \
           '' using 3 ti col, \
           '' using 4 ti col
