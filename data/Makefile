EVALDATS=smw.dat smr.dat lgw.dat lgr.dat mxw.dat mxr.dat
EVALSVGS=$(patsubst %.dat,%.svg,$(EVALDATS)) evalkey.svg
#EVALPNGS=$(patsubst %.svg,%.png,$(EVALSVGS))
EVALPDFS=$(patsubst %.svg,%.pdf,$(EVALSVGS))
#EVALEPSS=$(patsubst %.svg,%.eps,$(EVALSVGS))

CMPDATS=cmr.dat csr.dat clr.dat
CMPSVGS=$(patsubst %.dat,%.svg,$(CMPDATS)) cmpkey.svg
#CMPPNGS=$(patsubst %.svg,%.png,$(CMPSVGS))
CMPPDFS=$(patsubst %.svg,%.pdf,$(CMPSVGS))
#CMPEPSS=$(patsubst %.svg,%.eps,$(CMPSVGS))

PRACDATS=untar.dat cp.dat wc.dat tdfsio.dat lrdgen.dat lr.dat kmeans.dat
PRACSVGS=$(patsubst %.dat,%.svg,$(PRACDATS)) prackey.svg
#PRACPNGS=$(patsubst %.svg,%.png,$(PRACSVGS))
PRACPDFS=$(patsubst %.svg,%.pdf,$(PRACSVGS))
#PRACEPSS=$(patsubst %.svg,%.eps,$(PRACSVGS))

CTXDATS=ctxsws.dat ctxswl.dat ctxswm.dat
CTXSVGS=$(patsubst %.dat,%.svg,$(CTXDATS)) ctxswkey.svg
#CTXPNGS=$(patsubst %.svg,%.png,$(CTXSVGS))
CTXPDFS=$(patsubst %.svg,%.pdf,$(CTXSVGS))
#CTXEPSS=$(patsubst %.svg,%.eps,$(CTXSVGS))

FUSECMPDATS=fuseCompare.dat fuseCompareLg.dat
FUSECMPSVGS=$(patsubst %.dat,%.svg,$(FUSECMPDATS))
#FUSECMPPNGS=$(patsubst %.svg,%.png,$(FUSECMPSVGS))
FUSECMPPDFS=$(patsubst %.svg,%.pdf,$(FUSECMPSVGS))
#FUSECMPEPSS=$(patsubst %.svg,%.eps,$(FUSECMPSVGS))

FDSTASHDATS=fdstash.dat
FDSTASHSVGS=$(patsubst %.dat,%.svg,$(FDSTASHDATS))
#FDSTASHPNGS=$(patsubst %.svg,%.png,$(FDSTASHSVGS))
FDSTASHPDFS=$(patsubst %.svg,%.pdf,$(FDSTASHSVGS))
#FDSTASHEPSS=$(patsubst %.svg,%.eps,$(FDSTASHSVGS))

MMAPDATS=mmap.dat
MMAPSVGS=$(patsubst %.dat,%.svg,$(MMAPDATS))
MMAPPDFS=$(patsubst %.svg,%.pdf,$(MMAPSVGS))

TPCBBDATS=tpcbb.dat tpcbbkey.dat tpcbbnorm.dat tpcbbnormkey.dat
TPCBBSVGS=$(patsubst %.dat,%.svg,$(TPCBBDATS))
TPCBBPDFS=$(patsubst %.svg,%.pdf,$(TPCBBSVGS))

all: fuseCompare eval cmp prac ctx fdstash mmap tpcbb

fuseCompare: ${FUSECMPSVGS} ${FUSECMPPNGS} ${FUSECMPPDFS} ${FUSECMPEPSS}

eval: ${EVALSVGS} ${EVALPNGS} ${EVALPDFS} ${EVALEPSS}

cmp: ${CMPSVGS} ${CMPPNGS} ${CMPPDFS} ${CMPEPSS}

prac: ${PRACSVGS} ${PRACPNGS} ${PRACPDFS} ${PRACEPSS}

ctx: ${CTXSVGS} ${CTXPNGS} ${CTXPDFS} ${CTXEPSS}

fdstash: ${FDSTASHSVGS} ${FDSTASHPNGS} ${FDSTASHPDFS} ${FDSTASHEPSS}

mmap: ${MMAPSVGS} ${MMAPPDFS}

tpcbb: ${TPCBBSVGS} ${TPCBBPDFS}

%.pdf: %.svg
	inkscape $< --export-pdf=$@
	@#rsvg-convert -f pdf $< $@

%.eps: %.svg
	inkscape $< -E $@ --export-ignore-filters --export-ps-level=3

%.png: %.svg
	convert -density 1200 $< $@

evalkey.svg: evalkey.dat evalkey.plt 
	gnuplot -e "dat_file='$<'; out_file='$@';" evalkey.plt

cmpkey.svg: cmpkey.dat cmpkey.plt 
	gnuplot -e "dat_file='$<'; out_file='$@';" cmpkey.plt

prackey.svg: prackey.dat prackey.plt 
	gnuplot -e "dat_file='$<'; out_file='$@';" prackey.plt

smw.svg: smw.dat eval.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Small File Write'" eval.plt

smr.svg: smr.dat eval.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Small File Read'" eval.plt

lgw.svg: lgw.dat eval.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Large File Write'" eval.plt

lgr.svg: lgr.dat eval.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Large File Read'" eval.plt

mxw.svg: mxw.dat eval.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Mixed File Write'" eval.plt

mxr.svg: mxr.dat eval.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Mixed File Read'" eval.plt

cmr.svg: cmr.dat cmplm.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Mixed File'" cmplm.plt

csr.svg: csr.dat cmpls.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Small File'" cmpls.plt

clr.svg: clr.dat cmpll.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Large File'" cmpll.plt

untar.svg: untar.dat prac.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle=\"Decompress\nLinux Kernel\"; pracTitle=\"Runtime (sec)\"" prac.plt

cp.svg: cp.dat prac.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle=\"Backup Accross\nFile Systems\"; pracTitle=\"Runtime (sec)\"" prac.plt

wc.svg: wc.dat prac.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle=\"Spark\nWord Count\"; pracTitle=\"Runtime (min)\"" prac.plt

lrdgen.svg: lrdgen.dat prac.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle=\"Linear Regression\nData Generation\"; pracTitle=\"Runtime (sec)\"" prac.plt

lr.svg: lr.dat prac.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle=\"Linear Regression\"; pracTitle=\"Runtime (sec)\"" prac.plt

kmeans.svg: kmeans.dat prac.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle=\"KMeans\"; pracTitle=\"Runtime (sec)\"" prac.plt

tdfsio.svg: tdfsio.dat prac.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle=\"Hadoop\nTestDFSIO\"; pracTitle=\"Runtime (min)\"" prac.plt

ctxsws.svg: ctxsws.dat ctxsw.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle=\"Small File Context Switches\"" ctxsw.plt

ctxswl.svg: ctxswl.dat ctxsw.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle=\"Large File Context Switches\"" ctxsw.plt

ctxswm.svg: ctxswm.dat ctxsw.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle=\"Mixed File Context Switches\"" ctxsw.plt

ctxswkey.svg: ctxswkey.dat ctxswkey.plt 
	gnuplot -e "dat_file='$<'; out_file='$@';" ctxswkey.plt

fuseCompare.svg: fuseCompare.dat fuseCompare.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Small File'" fuseCompare.plt

fuseCompareLg.svg: fuseCompareLg.dat fuseCompareLg.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Large File'" fuseCompareLg.plt

fdstash.svg: fdstash.dat fdstash.plt
	gnuplot -e "dat_file='$<'; out_file='$@';" fdstash.plt

mmap.svg: mmap.dat mmap.plt
	gnuplot -e "dat_file='$<'; out_file='$@';" mmap.plt

tpcbb.svg: tpcbb.dat tpcbb.plt
	gnuplot -e "dat_file='$<'; out_file='$@';" tpcbb.plt

tpcbbkey.svg: tpcbbkey.dat tpcbbkey.plt
	gnuplot -e "dat_file='$<'; out_file='$@';" tpcbbkey.plt

tpcbbnorm.svg: tpcbbnorm.dat tpcbbnorm.plt
	gnuplot -e "dat_file='$<'; out_file='$@';" tpcbbnorm.plt

tpcbbnormkey.svg: tpcbbnormkey.dat tpcbbnormkey.plt
	gnuplot -e "dat_file='$<'; out_file='$@';" tpcbbnormkey.plt

clean:
	rm -f *.svg
	rm -f *.png
	rm -f *.pdf
	rm -f *.eps
