# Set formatting
set output out_file         # Set the output path

# Set the font to something pleasing
set term svg fname "Times,18" size 1200, 50
set key outside center horizontal center   # Place the key in an appropriate place
set key samplen 2
set key width -1

set style data histogram
#set style fill solid 0.25 border
set style fill pattern border

set xrange [100:101]
set yrange [100:101]

set notitle
set noborder
set noxtics
set noytics
set notitle
set noxlabel
set noylabel

plot dat_file using 2:xtic(1) ti col lc 6 fs pattern 6, \
           '' using 3 ti col lc 1 fs pattern 1, \
           '' using 4 ti col lc 7 fs pattern 7, \
           '' using 5 ti col lc 2 fs pattern 2, \
           '' using 6 ti col lc 3 fs pattern 4, \
           '' using 7 ti col lc 4 fs pattern 5
