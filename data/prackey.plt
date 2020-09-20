# Set formatting
set output out_file         # Set the output path

# Set the font to something pleasing
set term svg fname "Times,21" size 900, 50
set key outside center horizontal left   # Place the key in an appropriate place
set key samplen 2
#set key width -9.5

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

#plot dat_file using 2:xtic(1) ti col lc 6 fs pattern 6, \
#           '' using 3 ti col lc 1 fs pattern 1, \
#           '' using 4 ti col lc 2 fs pattern 2, \
#           '' using 5 ti col lc 3 fs pattern 4, \
#           '' using 6 ti col lc 4 fs pattern 5

plot dat_file using 2:xtic(1) ti col lc 6 fs pattern 6, \
           '' using 4 ti col lc 2 fs pattern 2, \
           '' using 5 ti col lc 3 fs pattern 4
