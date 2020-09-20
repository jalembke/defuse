# Set formatting
set output out_file         # Set the output path

# Set the font to something pleasing
set term svg fname "Times,21" size 798, 100
set key outside center horizontal left   # Place the key in an appropriate place
set key samplen 2
set key width -3.5

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

plot dat_file using 2:xtic(1) ti col fs pattern 1, \
           '' using 3 ti col fs pattern 2, \
           '' using 4 ti col fs pattern 4, \
           '' using 5 ti col fs pattern 5
