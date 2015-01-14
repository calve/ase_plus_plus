BINDIR="bin/"
SRCDIR="src/"
BINARIES= mkhd dmps frmt vm mkvol dvol mknfs dfs if_pfile if_nfile if_cfile if_dfile


all: bin-dir
	make -C $(SRCDIR) all

bin-dir:
	mkdir -p $(BINDIR)

realclean:
	rm -rf ./bin
	make -C $(SRCDIR) realclean
