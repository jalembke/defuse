EVALDATS=smw.dat smr.dat lgw.dat lgr.dat mxw.dat mxr.dat
EVALSVGS=$(patsubst %.dat,%.svg,$(EVALDATS)) evalkey.svg defusekey.svg fusekey.svg fusediokey.svg ldpkey.svg
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
CTXSVGS=$(patsubst %.dat,%.svg,$(CTXDATS)) ctxswkey.svg directkey.svg defusekey.svg fusekey.svg fusediokey.svg ldpkey.svg
#CTXPNGS=$(patsubst %.svg,%.png,$(CTXSVGS))
CTXPDFS=$(patsubst %.svg,%.pdf,$(CTXSVGS))
#CTXEPSS=$(patsubst %.svg,%.eps,$(CTXSVGS))

FUSECMPDATS=fuseCompareSmall.dat fuseCompareMixed.dat fuseCompareLarge.dat fuseCompareCtxSmall.dat fuseCompareCtxMixed.dat fuseCompareCtxLarge.dat
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

TPCBBDATS=tpcbb.dat tpcbbkey.dat tpcbbnorm.dat tpcbbnorm2.dat tpcbbnormkey.dat
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
	pdfcrop --margins 1 $@ $@

%.eps: %.svg
	inkscape $< -E $@ --export-ignore-filters --export-ps-level=3

%.png: %.svg
	convert -density 1200 $< $@

evalkey.svg: evalkey.dat evalkey.plt 
	gnuplot -e "dat_file='$<'; out_file='$@';" evalkey.plt

defusekey.svg: defusekey.dat defusekey.plt 
	gnuplot -e "dat_file='$<'; out_file='$@';" defusekey.plt

directkey.svg: directkey.dat directkey.plt 
	gnuplot -e "dat_file='$<'; out_file='$@';" directkey.plt

ldpkey.svg: ldpkey.dat ldpkey.plt 
	gnuplot -e "dat_file='$<'; out_file='$@';" ldpkey.plt

fusediokey.svg: fusediokey.dat fusediokey.plt 
	gnuplot -e "dat_file='$<'; out_file='$@';" fusediokey.plt

fusekey.svg: fusekey.dat fusekey.plt 
	gnuplot -e "dat_file='$<'; out_file='$@';" fusekey.plt

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

cmr.svg: cmr.dat cmpl.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Mixed File'; yformat='%.0f'; toMB='1';" cmpl.plt

csr.svg: csr.dat cmpl.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Small File'; yformat='%.1f'; toMB='1000';" cmpl.plt

clr.svg: clr.dat cmpl.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Large File'; yformat='%.0f'; toMB='1';" cmpl.plt

untar.svg: untar.dat prac.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle=\"Decompress\nLinux Kernel\"; pracTitle=\"Runtime (s)\"; yformat=\"%.1f s\";" prac.plt

cp.svg: cp.dat prac.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle=\"Backup Accross\nFile Systems\"; pracTitle=\"Runtime (s)\"; yformat=\"%.1f s\";" prac.plt

wc.svg: wc.dat prac.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle=\"Spark\nWord Count\"; pracTitle=\"Runtime (m)\"; yformat=\"%.0f m\";" prac.plt

lrdgen.svg: lrdgen.dat prac.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle=\"Linear Regression\nData Generation\"; pracTitle=\"Runtime (s)\"; yformat=\"%.0f s\";" prac.plt

lr.svg: lr.dat prac.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle=\"Linear Regression\"; pracTitle=\"Runtime (s)\"; yformat=\"%.0f s\";" prac.plt

kmeans.svg: kmeans.dat prac.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle=\"KMeans\"; pracTitle=\"Runtime (s)\"; yformat=\"%.0f s\";" prac.plt

tdfsio.svg: tdfsio.dat prac.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle=\"Hadoop\nTestDFSIO\"; pracTitle=\"Runtime (m)\"; yformat=\"%.0f m\";" prac.plt

ctxsws.svg: ctxsws.dat ctxsw.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle=\"Small File Context Switches\"" ctxsw.plt

ctxswl.svg: ctxswl.dat ctxsw.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle=\"Large File Context Switches\"" ctxsw.plt

ctxswm.svg: ctxswm.dat ctxsw.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle=\"Mixed File Context Switches\"" ctxsw.plt

ctxswkey.svg: ctxswkey.dat ctxswkey.plt 
	gnuplot -e "dat_file='$<'; out_file='$@';" ctxswkey.plt

fuseCompareSmall.svg: fuseCompareSmall.dat fuseCompare.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Small File'; yformat='%.2f'; yticSpread='0.25,0.25';" fuseCompare.plt

fuseCompareMixed.svg: fuseCompareMixed.dat fuseCompare.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Mixed File'; yformat='%.0f'; yticSpread='50,50';" fuseCompare.plt

fuseCompareLarge.svg: fuseCompareLarge.dat fuseCompare.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Large File'; yformat='%.0f'; yticSpread='50,50';" fuseCompare.plt

fuseCompareCtxSmall.svg: fuseCompareCtxSmall.dat fuseCompareCtx.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Small File'" fuseCompareCtx.plt

fuseCompareCtxMixed.svg: fuseCompareCtxMixed.dat fuseCompareCtx.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Mixed File'" fuseCompareCtx.plt

fuseCompareCtxLarge.svg: fuseCompareCtxLarge.dat fuseCompareCtx.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Large File'" fuseCompareCtx.plt

fdstash.svg: fdstash.dat fdstash.plt
	gnuplot -e "dat_file='$<'; out_file='$@';" fdstash.plt

mmap.svg: mmap.dat mmap.plt
	gnuplot -e "dat_file='$<'; out_file='$@';" mmap.plt
	inkscape --actions="select-all;transform-rotate:90;FileSave;FileClose" --batch-process $@

tpcbb.svg: tpcbb.dat tpcbb.plt
	gnuplot -e "dat_file='$<'; out_file='$@';" tpcbb.plt

tpcbbkey.svg: tpcbbkey.dat tpcbbkey.plt
	gnuplot -e "dat_file='$<'; out_file='$@';" tpcbbkey.plt

tpcbbnorm.svg: tpcbbnorm.dat tpcbbnorm.plt
	gnuplot -e "dat_file='$<'; out_file='$@';" tpcbbnorm.plt

tpcbbnorm2.svg: tpcbbnorm2.dat tpcbbnorm2.plt
	gnuplot -e "dat_file='$<'; out_file='$@';" tpcbbnorm2.plt

tpcbbnormkey.svg: tpcbbnormkey.dat tpcbbnormkey.plt
	gnuplot -e "dat_file='$<'; out_file='$@';" tpcbbnormkey.plt

clean:
	rm -f *.svg
	rm -f *.png
	rm -f *.pdf
	rm -f *.eps
