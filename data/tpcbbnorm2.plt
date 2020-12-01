set output out_file         # Set the output path
set term svg fname "Times,19" size 700, 250
set grid ytics              # Turn the grid on for yaxis

#unset key
set key top left
set key samplen 2
set key spacing 0.75

#set title gtitle offset 0,-0.7
set auto x
set style data histogram
set style histogram cluster gap 1
#set style fill solid 0.25 border
set style fill pattern border
set boxwidth 0.8

set xtic scale 0
#set xtics rotate by 20 right
set xtics offset 0,0.5
set ytics offset graph 0.015,-0.02
set ytics border in nomirror scale 0.4

set xrange [-0.5:6.5]

#set tmargin at screen 0.97
#set rmargin at screen 0.99
#set lmargin at screen 0.23
#set bmargin at screen 0.25

#set ylabel "Runtime (% of direct mount)" offset 3.0,-0.5
set ylabel "Normalized runtime" offset 2.7

# To show direct mount value
f(x) = 1

# 2, 3, 4, 5 are the indexes of the columns; 'fc' stands for 'fillcolor'
plot dat_file using ($2/100):xtic(1) ti col fs pattern 2 lc 2, \
           '' using ($3/100) ti col fs pattern 4 lc 3, \
           f(x) lw 2 lc black dt 2 title "Direct mount"
