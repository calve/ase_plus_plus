BINDIR="bin/"
SRCDIR="tpfs/"
BINARIES= mkhd dmps frmt vm mkvol dvol mknfs dfs if_pfile if_nfile if_cfile if_dfile


all: bin-dir
	make -C $(SRCDIR)
	cd $
	mv $(SRCDIR)/ $(BINDIR)

bin-dir:
	mkdir -p $(BINDIR)
