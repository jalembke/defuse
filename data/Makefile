EVALDATS=smw.dat smr.dat lgw.dat lgr.dat mxw.dat mxr.dat
EVALSVGS=$(patsubst %.dat,%.svg,$(EVALDATS)) evalkey.svg
EVALPNGS=$(patsubst %.svg,%.png,$(EVALSVGS))
EVALPDFS=$(patsubst %.svg,%.pdf,$(EVALSVGS))
EVALEPSS=$(patsubst %.svg,%.eps,$(EVALSVGS))

CMPDATS=cmr.dat csr.dat clr.dat
CMPSVGS=$(patsubst %.dat,%.svg,$(CMPDATS)) cmpkey.svg
CMPPNGS=$(patsubst %.svg,%.png,$(CMPSVGS))
CMPPDFS=$(patsubst %.svg,%.pdf,$(CMPSVGS))
CMPEPSS=$(patsubst %.svg,%.eps,$(CMPSVGS))

PRACDATS=untar.dat cp.dat
PRACSVGS=$(patsubst %.dat,%.svg,$(PRACDATS)) prackey.svg
PRACPNGS=$(patsubst %.svg,%.png,$(PRACSVGS))
PRACPDFS=$(patsubst %.svg,%.pdf,$(PRACSVGS))
PRACEPSS=$(patsubst %.svg,%.eps,$(PRACSVGS))

CTXDATS=ctxsws.dat ctxswl.dat ctxswm.dat
CTXSVGS=$(patsubst %.dat,%.svg,$(CTXDATS)) ctxswkey.svg
CTXPNGS=$(patsubst %.svg,%.png,$(CTXSVGS))
CTXPDFS=$(patsubst %.svg,%.pdf,$(CTXSVGS))
CTXEPSS=$(patsubst %.svg,%.eps,$(CTXSVGS))

FUSECMPDATS=fuseCompare.dat
FUSECMPSVGS=$(patsubst %.dat,%.svg,$(FUSECMPDATS))
FUSECMPPNGS=$(patsubst %.svg,%.png,$(FUSECMPSVGS))
FUSECMPPDFS=$(patsubst %.svg,%.pdf,$(FUSECMPSVGS))
FUSECMPEPSS=$(patsubst %.svg,%.eps,$(FUSECMPSVGS))

all: fuseCompare eval cmp prac ctx

fuseCompare: ${FUSECMPSVGS} ${FUSECMPPNGS} ${FUSECMPPDFS} ${FUSECMPEPSS}

eval: ${EVALSVGS} ${EVALPNGS} ${EVALPDFS} ${EVALEPSS}

cmp: ${CMPSVGS} ${CMPPNGS} ${CMPPDFS} ${CMPEPSS}

prac: ${PRACSVGS} ${PRACPNGS} ${PRACPDFS} ${PRACEPSS}

ctx: ${CTXSVGS} ${CTXPNGS} ${CTXPDFS} ${CTXEPSS}

%.pdf: %.svg
	inkscape $< --export-pdf=$@

%.eps: %.svg
	inkscape $< -E $@ --export-ignore-filters --export-ps-level=3

%.png: %.svg
	convert $< $@

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

cmr.svg: cmr.dat cmp.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Mixed File'" cmpl.plt

csr.svg: csr.dat cmpl.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Small File'" cmpl.plt

clr.svg: clr.dat cmp.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Large File'" cmpl.plt

untar.svg: untar.dat prac.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle=\"Decompress\nLinux Kernel\"" prac.plt

cp.svg: cp.dat prac.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle=\"Backup Accross\nFiile Systems\"" prac.plt

ctxsws.svg: ctxsws.dat ctxsw.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle=\"Small File Context Switches\"" ctxsw.plt

ctxswl.svg: ctxswl.dat ctxsw.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle=\"Large File Context Switches\"" ctxsw.plt

ctxswm.svg: ctxswm.dat ctxsw.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle=\"Mixed File Context Switches\"" ctxsw.plt

ctxswkey.svg: ctxswkey.dat ctxswkey.plt 
	gnuplot -e "dat_file='$<'; out_file='$@';" ctxswkey.plt

fuseCompare.svg: fuseCompare.dat fuseCompare.plt
	gnuplot -e "dat_file='$<'; out_file='$@';" fuseCompare.plt

clean:
	rm -f *.svg
	rm -f *.png
	rm -f *.pdf
	rm -f *.eps
