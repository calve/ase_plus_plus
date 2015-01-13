# $Id: Makefile,v 1.1 2002/12/06 14:25:18 marquet Exp $
##############################################################################

ROOTDIR=../

CC	= gcc
CFLAGS	= -Wall -ansi -pedantic
CFLAGS  += -std=iso9899:1999
CFLAGS  += -g -m32
LIBDIR  = -L$(ROOTDIR)/lib
INCDIR  = -I$(ROOTDIR)/include
LIBS    = -lhardware

###------------------------------
### Main targets 
###------------------------------------------------------------
BINARIES= mkhd dmps frmt vm mkvol dvol mknfs dfs if_pfile if_nfile if_cfile if_dfile
OBJECTS	= $(addsuffix .o,\
	  drive mbr tools ifile mount inode superbloc)

all: $(BINARIES) $(OBJECTS)

###------------------------------
### Binaries
###------------------------------------------------------------
mkhd: mkhd.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBDIR) $(LIBS)
dmps: dmps.o drive.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBDIR) $(LIBS)
frmt: frmt.o drive.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBDIR) $(LIBS)
vm: vm.o mbr.o drive.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBDIR) $(LIBS)
mkvol: mkvol.o mbr.o drive.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBDIR) $(LIBS)
dvol: dvol.o mbr.o drive.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBDIR) $(LIBS)
mknfs: mknfs.o mbr.o drive.o superbloc.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBDIR) $(LIBS)
dfs: dfs.o mbr.o drive.o superbloc.o current.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBDIR) $(LIBS)
if_cfile: if_cfile.o $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBDIR) $(LIBS)
if_nfile: if_nfile.o $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBDIR) $(LIBS)
if_dfile: if_dfile.o $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBDIR) $(LIBS)
if_pfile: if_pfile.o ifile.o $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBDIR) $(LIBS)

###------------------------------
### #include dependences 
###------------------------------------------------------------
# you may fill these lines with "make depend"
mkhd.o: mkhd.c 
dmps.o: dmps.c 
frmt.o: frmt.c 
vm.o: vm.c
current.o: current.c 

drive.o: drive.c drive.h
mbr.o: mbr.c drive.h mbr.h
mkvol.o: mbr.c mbr.h drive.h
dvol.o: mbr.c mbr.h drive.h
mknfs.o: mbr.h drive.h superbloc.h mknfs.c
dfs.o: mbr.h drive.h superbloc.h dfs.c current.c
superbloc.o: mbr.h drive.h superbloc.h superbloc.c
inode.o:inode.c inode.h mbr.h superbloc.h drive.h

if_cfile.o: if_cfile.c ifile.h inode.h tools.h mount.h
if_dfile.o: if_dfile.c ifile.h inode.h tools.h mount.h
if_nfile.o: if_nfile.c ifile.h inode.h tools.h mount.h
if_pfile.o: if_pfile.c ifile.h inode.h tools.h mount.h
ifile.o: ifile.c inode.h tools.h ifile.h 
mount.o: mount.c config.h tools.h inode.h mbr.h superbloc.h drive.h
tools.o: tools.c tools.h


%.o: %.c
	$(CC) $(CFLAGS) -c $< $(INCDIR)

###------------------------------
### Misc.
###------------------------------------------------------------
.PHONY: clean realclean depend
clean:
	$(RM) *.o $(BINARIES)
realclean: clean 
	$(RM) vdiskA.bin vdiskB.bin
depend : 
	$(CC) -MM $(INCDIR) *.c

