EVALDATS=smw.dat smr.dat lgw.dat lgr.dat mxw.dat mxr.dat
EVALSVGS=$(patsubst %.dat,%.svg,$(EVALDATS)) evalkey.svg

CMPDATS=cmr.dat csr.dat clr.dat
CMPSVGS=$(patsubst %.dat,%.svg,$(CMPDATS)) cmpkey.svg

PRACDATS=untar.dat cp.dat
PRACSVGS=$(patsubst %.dat,%.svg,$(PRACDATS)) prackey.svg

all: fuseCompare.svg eval cmp prac

eval: ${EVALSVGS}

cmp: ${CMPSVGS}

prac: ${PRACSVGS}

evalkey.svg: evalkey.dat evalkey.plt 
	gnuplot -e "dat_file='$<'; out_file='$@';" evalkey.plt
	convert evalkey.svg evalkey.png

cmpkey.svg: cmpkey.dat cmpkey.plt 
	gnuplot -e "dat_file='$<'; out_file='$@';" cmpkey.plt
	convert cmpkey.svg cmpkey.png

prackey.svg: prackey.dat prackey.plt 
	gnuplot -e "dat_file='$<'; out_file='$@';" prackey.plt
	convert prackey.svg prackey.png

smw.svg: smw.dat eval.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Small File Write'" eval.plt
	convert smw.svg smw.png

smr.svg: smr.dat eval.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Small File Read'" eval.plt
	convert smr.svg smr.png

lgw.svg: lgw.dat eval.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Large File Write'" eval.plt
	convert lgw.svg lgw.png

lgr.svg: lgr.dat eval.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Large File Read'" eval.plt
	convert lgr.svg lgr.png

mxw.svg: mxw.dat eval.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Mixed File Write'" eval.plt
	convert mxw.svg mxw.png

mxr.svg: mxr.dat eval.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Mixed File Read'" eval.plt
	convert mxr.svg mxr.png

cmr.svg: cmr.dat cmp.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Mixed File'" cmp.plt
	convert cmr.svg cmr.png

csr.svg: csr.dat cmpl.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Small File'" cmpl.plt
	convert csr.svg csr.png

clr.svg: clr.dat cmp.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Large File'" cmp.plt
	convert clr.svg clr.png

untar.svg: untar.dat prac.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle=\"Decompress\nLinux Kernel\"" prac.plt
	convert untar.svg untar.png

cp.svg: cp.dat prac.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle=\"Backup Accross\nFiile Systems\"" prac.plt
	convert cp.svg cp.png

fuseCompare.svg: fuseCompare.dat fuseCompare.plt
	gnuplot -e "dat_file='$<'; out_file='$@';" fuseCompare.plt
	convert fuseCompare.svg fuseCompare.png

clean:
	rm -f *.svg
	rm -f *.png
