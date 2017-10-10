EVALDATS=smw.dat smr.dat lgw.dat lgr.dat mxw.dat mxr.dat
EVALSVGS=$(patsubst %.dat,%.svg,$(EVALDATS)) evalkey.svg

CMPDATS=cmr.dat csr.dat clr.dat
CMPSVGS=$(patsubst %.dat,%.svg,$(CMPDATS)) evalkey.svg

all: fuseCompare.svg eval cmp

eval: ${EVALSVGS}

cmp: ${CMPSVGS}

evalkey.svg: evalkey.dat evalkey.plt 
	gnuplot -e "dat_file='$<'; out_file='$@';" evalkey.plt
	convert evalkey.svg evalkey.png

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
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Mixed File Read'" cmp.plt
	convert cmr.svg cmr.png

csr.svg: csr.dat cmp.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Small File Read'" cmp.plt
	convert csr.svg csr.png

clr.svg: clr.dat cmp.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Large File Read'" cmp.plt
	convert clr.svg clr.png

fuseCompare.svg: fuseCompare.dat fuseCompare.plt
	gnuplot -e "dat_file='$<'; out_file='$@';" fuseCompare.plt
	convert fuseCompare.svg fuseCompare.png

clean:
	rm -f *.svg
	rm -f *.png
