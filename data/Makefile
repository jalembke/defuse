EVALDATS=sm_w.dat sm_r.dat lg_w.dat lg_r.dat mx_w.dat mx_r.dat
EVALSVGS=$(patsubst %.dat,%.svg,$(EVALDATS))

all: fuse_compare.svg eval

eval: ${EVALSVGS}

sm_w.svg: sm_w.dat eval.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Small File Write'" eval.plt

sm_r.svg: sm_r.dat eval.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Small File Read'" eval.plt

lg_w.svg: lg_w.dat eval.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Large File Write'" eval.plt

lg_r.svg: lg_r.dat eval.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Large File Read'" eval.plt

mx_w.svg: mx_w.dat eval.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Mixed File Write'" eval.plt

mx_r.svg: mx_r.dat eval.plt
	gnuplot -e "dat_file='$<'; out_file='$@'; gtitle='Mixed File Read'" eval.plt

fuse_compare.svg: fuse_compare.dat fuse_compare.plt
	gnuplot -e "dat_file='$<'; out_file='$@';" fuse_compare.plt

clean:
	rm -f *.svg
