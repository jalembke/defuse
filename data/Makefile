SVGFILES=$(patsubst %.dat,%.svg,$(wildcard *.dat))
PNGFILES=$(patsubst %.dat,%.png,$(wildcard *.dat))

all: ${SVGFILES}

%.svg: %.dat %.plt
	gnuplot -e "dat_file='$<'; out_file='$@';" $*.plt
	convert $*.svg $*.png

clean:
	rm -f ${SVGFILES}
	rm -f ${PNGFILES}
