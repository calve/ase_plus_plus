AUTHOR_NAME = debusschere
FIELD_NAME = ase
TP_INDEX = 3
SHELL = /bin/bash

# $Id: Makefile,v 1.2 2004/10/12 09:06:17 marquet Exp $
##############################################################################

ROOTDIR=.

CC	= gcc
CFLAGS	= -Wall -ansi -pedantic -m32
CFLAGS  += -g
SRCDIR = src
BINDIR = bin
LIBDIR  = $(ROOTDIR)/lib
INCDIR  = -I$(ROOTDIR)/include
LIBS    = -L$(LIBDIR) -lhardware

###------------------------------
### Main targets
###------------------------------------------------------------
BINARIES= mkvol dvol
OBJECTS= $(addsuffix .o,\
	  mkhd)

all: $(BINARIES) $(OBJECTS)


###------------------------------
### Main rules
###------------------------------------------------------------
lib/drive.o: $(SRCDIR)/drive.c
	$(CC) $(CFLAGS) -o $@ -c $(SRCDIR)/drive.c $(INCDIR)

lib/block.o: $(SRCDIR)/block.c $(SRCDIR)/drive.c
	$(CC) $(CFLAGS) -o $@ -c $(SRCDIR)/block.c $(INCDIR)

lib/vol.o: $(SRCDIR)/vol.c $(SRCDIR)/block.c $(SRCDIR)/drive.c
	$(CC) $(CFLAGS) -o $@ -c $(SRCDIR)/vol.c $(INCDIR)

lib/inode.o: $(SRCDIR)/inode.c $(SRCDIR)/vol.c $(SRCDIR)/block.c $(SRCDIR)/drive.c
	$(CC) $(CFLAGS) -o $@ -c $(SRCDIR)/inode.c $(INCDIR)

mkhd.o: $(SRCDIR)/mkhd.c
	$(CC) $(CFLAGS) -o $(LIBDIR)/$@ -c $(SRCDIR)/mkhd.c $(INCDIR)

mkhd: bindir $(SRCDIR)/mkhd.o
	$(CC) $(CFLAGS) -o $(BINDIR)/mkhd $(SRCDIR)/mkhd.o $(LIBS)

testdrive.o: $(SRCDIR)/testdrive.c $(LIBDIR)/drive.o
	$(CC) $(CFLAGS) -o $(LIBDIR)/$@ -c $(SRCDIR)/testdrive.c $(INCDIR)

testdrive: bindir $(SRCDIR)/testdrive.o lib/drive.o
	$(CC) $(CFLAGS) -o $(BINDIR)/testdrive $(SRCDIR)/testdrive.o lib/drive.o  $(LIBS)

format.o: $(SRCDIR)/format.c $(SRCDIR)/drive.c
	$(CC) $(CFLAGS) -o $(LIBDIR)/$@ -c $(SRCDIR)/format.c $(INCDIR) $(LIBS)

format: bindir $(SRCDIR)/format.o lib/drive.o
	$(CC) $(CFLAGS) -o $(BINDIR)/format $(SRCDIR)/format.o lib/drive.o $(LIBS)

dump.o: $(SRCDIR)/dump.c $(SRCDIR)/drive.c
	$(CC) $(CFLAGS) -o $(LIBDIR)/$@ -c $(SRCDIR)/dump.c $(INCDIR)

dump: bindir $(SRCDIR)/dump.o lib/drive.o
	$(CC) $(CFLAGS) -o $(BINDIR)/dump $(SRCDIR)/dump.o lib/drive.o $(LIBS)

mkvol.o: $(SRCDIR)/mkvol.c
	$(CC) $(CFLAGS) -o $(LIBDIR)/$@ -c $(SRCDIR)/mkvol.c $(INCDIR) $(LIBS)

mkvol: bindir $(SRCDIR)/mkvol.o lib/vol.o lib/block.o lib/drive.o
	$(CC) $(CFLAGS) -o $(BINDIR)/mkvol $(SRCDIR)/mkvol.o lib/vol.o lib/block.o lib/drive.o $(LIBS)

dvol.o: $(SRCDIR)/dvol.c
	$(CC) $(CFLAGS) -o $(LIBDIR)/$@ -c $(SRCDIR)/dvol.c $(INCDIR) $(LIBS)

dvol: bindir $(SRCDIR)/dvol.o lib/vol.o lib/block.o lib/drive.o
	$(CC) $(CFLAGS) -o $(BINDIR)/dvol $(SRCDIR)/dvol.o lib/vol.o lib/block.o lib/drive.o $(LIBS)

bindir:
	mkdir -p $(BINDIR)

test: $(BINDIR)/dvol $(BINDIR)/mkvol
	rm vdiskA.bin
	$(BINDIR)/dvol
	$(BINDIR)/mkvol --cylinder 0 --sector 1 --size 16 --type base --name moustique --serial 42
	$(BINDIR)/dvol
	$(BINDIR)/mkvol --cylinder 2  --sector 1 --size 17 --type annex --name carotte --serial 33
	$(BINDIR)/dvol
	$(BINDIR)/mkvol --cylinder 4 --sector 1 --size 18 --type other --name begonia --serial 22
	$(BINDIR)/dvol
	@echo ""
	@echo "At this point, there should have been 3 different drive created"
	@echo "We will now create several inodes"
	@echo ""
	number=1 ; while [[ $$number -le 20 ]] ; do \
	    $(BINDIR)/if_create ; \
	    ((number = number + 1)) ; \
	done
	@echo ""
	@echo "Disk should be full. Delete a few inodes"
	@echo ""
	number=1 ; while [[ $$number -le 10 ]] ; do \
	    echo "delete inode $$number" ; \
	    $(BINDIR)/if_delete $$number ; \
	    ((number = number + 1)) ; \
	done
	@echo ""
	@echo "Okay, reask for more inodes"
	@echo ""
	number=1 ; while [[ $$number -le 3 ]] ; do \
	    $(BINDIR)/if_create ; \
	    ((number = number + 1)) ; \
	done
	@echo ""
	@echo "We should have got some new inodes"
	@echo "Now copying some files"
	@echo ""
	$(BINDIR)/if_copy 2 < $(SRCDIR)/volume.h
	$(BINDIR)/if_copy 5 < README.md
	@echo ""
	@echo "Now dump them"
	@echo ""
	$(BINDIR)/if_dump 2
	$(BINDIR)/if_dump 5



###------------------------------
### Misc.
###------------------------------------------------------------
.PHONY: clean realclean depend
clean:
	$(RM) *.o $(BINARIES)
realclean: clean
	$(RM) vdiskA.bin vdiskB.bin
	$(RM) -Rdv $(BINDIR)

archive:
	git archive --output=$(AUTHOR_NAME)-$(FIELD_NAME)-$(TP_INDEX).tar.gz master
