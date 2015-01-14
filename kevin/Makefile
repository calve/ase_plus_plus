BINDIR="bin/"
SRCDIR="tpfs/"
BINARIES= mkhd dmps frmt vm mkvol dvol mknfs dfs if_pfile if_nfile if_cfile if_dfile


all: bin-dir
	make -C $(SRCDIR)
	cp hardware.ini ./$(BINDIR)
	cd $(BINDIR); ./mkhd

bin-dir:
	mkdir -p $(BINDIR)

realclean:
	rm -rf ./bin
	cd $(SRCDIR); make realclean
	cd ..
