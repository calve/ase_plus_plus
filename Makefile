BINDIR=bin
SRCDIR=src
BINARIES= mkhd dmps frmt vm mkvol dvol mknfs dfs if_pfile if_nfile if_cfile if_dfile
SHELL=bash

all: bin-dir
	make -C $(SRCDIR) all

bin-dir:
	mkdir -p $(BINDIR)

realclean:
	rm -rf ./bin
	make -C $(SRCDIR) realclean

test: export CURRENT_VOLUME = 1
test: $(BINDIR)/dvol $(BINDIR)/mkvol
	rm vdiskA.bin
	@echo ""
	@echo "Creating virtual hard drive"
	@echo ""
	$(BINDIR)/mkhd
	@echo ""
	@echo "Creating volumes"
	@echo ""
	$(BINDIR)/mkvol --cylinder 0 --sector 1 --size 16 --type base > /dev/null
	$(BINDIR)/mkvol --cylinder 2  --sector 1 --size 17 --type annex > /dev/null
	$(BINDIR)/mkvol --cylinder 4 --sector 1 --size 18 --type other > /dev/null
	@echo ""
	@echo "At this point, there should have been 3 different drive created. Dump the volume table"
	@echo ""
	$(BINDIR)/dvol
	@echo ""
	@echo "Create a filesystem on current_volume"
	@echo ""
	$(BINDIR)/mknfs --volume $(CURRENT_VOLUME) --serial 424242 --size 14
	@echo ""
	@echo "We will now create several inodes"
	@echo ""
	number=1 ; while [[ $$number -le 20 ]] ; do \
	    echo $$number | $(BINDIR)/if_nfile; \
	    ((number = number + 1)) ; \
	done
	@echo ""
	@echo "Disk should be full. Delete a few inodes"
	@echo ""
	number=1 ; while [[ $$number -le 10 ]] ; do \
	    echo "delete inode $$number" ; \
	    $(BINDIR)/if_dfile $$number ; \
	    ((number = number + 1)) ; \
	done
	@echo ""
	@echo "Okay, reask for more inodes"
	@echo ""
	number=1 ; while [[ $$number -le 3 ]] ; do \
	    $(BINDIR)/if_cfile ; \
	    ((number = number + 1)) ; \
	done
	@echo ""
	@echo "We should have got some new inodes"
	@echo "Now copying some files"
	@echo ""
	$(BINDIR)/if_nfile 2 < $(SRCDIR)/volume.h
	$(BINDIR)/if_nfile 5 < README.md
	@echo ""
	@echo "Now dump them"
	@echo ""
	$(BINDIR)/if_dump 2
	$(BINDIR)/if_dump 5
