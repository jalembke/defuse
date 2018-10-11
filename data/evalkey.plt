# Set formatting
set output out_file         # Set the output path

# Set the font to something pleasing
set term svg fname "Times,19" size 1000, 50
set key outside center horizontal center   # Place the key in an appropriate place
set key width -3

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

plot dat_file using 2:xtic(1) ti col fs pattern 1, \
           '' using 3 ti col fs pattern 2, \
           '' using 4 ti col fs pattern 4, \
           '' using 5 ti col fs pattern 5,
