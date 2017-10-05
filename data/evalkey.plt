# Set formatting
set output out_file         # Set the output path

# Set the font to something pleasing
set term svg fname "Times,19" size 800, 75
set key outside center horizontal left   # Place the key in an appropriate place

set style data histogram
set style fill solid 0.25 border

set xrange [100:101]
set yrange [100:101]

set notitle
set noborder
set noxtics
set noytics
set notitle
set noxlabel
set noylabel

plot dat_file using 2:xtic(1) ti col, \
           '' using 3 ti col, \
           '' using 4 ti col
