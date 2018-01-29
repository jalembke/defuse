# Set formatting
set output out_file         # Set the output path

# Set the font to something pleasing
set term svg fname "Times,21" size 1200, 100
set key outside center horizontal left   # Place the key in an appropriate place
set key width -5.5

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

plot dat_file using 2:xtic(1) ti col fc 6, \
           '' using 3 ti col fc 1, \
           '' using 4 ti col fc 2, \
           '' using 5 ti col fc 3, \
           '' using 6 ti col fc 4, \
           '' using 7 ti col fc 5, \
           '' using 8 ti col fc 8
