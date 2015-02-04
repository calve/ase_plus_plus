# $Id: Makefile,v 1.1 2002/12/06 14:25:18 marquet Exp $
##############################################################################

ROOTDIR=../

CC	= gcc
CFLAGS	= -Wall -ansi
CFLAGS  +=  -std=gnu99
CFLAGS  += -g -m32
BINDIR = $(ROOTDIR)/bin
LIBDIR  = -L$(ROOTDIR)/lib
INCDIR  = -I$(ROOTDIR)/include
LIBS    = -lhardware -lreadline

###------------------------------
### Main targets
###------------------------------------------------------------
BINARIES= mkhd dmps frmt vm mkvol dvol mknfs dfs if_pfile if_nfile if_cfile if_dfile shell
OBJECTS	= $(addsuffix .o,\
	  ctx hw drive mbr tools ifile mount inode superbloc dir file commons builtins)

all: $(BINARIES) $(OBJECTS)

###------------------------------
### Binaries
###------------------------------------------------------------
mkhd: mkhd.o
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $^ $(LIBDIR) $(LIBS)
dmps: dmps.o drive.o
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $^ $(LIBDIR) $(LIBS)
frmt: frmt.o drive.o
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $^ $(LIBDIR) $(LIBS)
vm: vm.o mbr.o drive.o
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $^ $(LIBDIR) $(LIBS)
mkvol: mkvol.o mbr.o drive.o
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $^ $(LIBDIR) $(LIBS)
dvol: dvol.o mbr.o drive.o
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $^ $(LIBDIR) $(LIBS)
mknfs: mknfs.o mbr.o drive.o superbloc.o ifile.o inode.o tools.o
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $^ $(LIBDIR) $(LIBS)
dfs: dfs.o mbr.o drive.o superbloc.o ifile.o inode.o tools.o current.o
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $^ $(LIBDIR) $(LIBS)
if_cfile: if_cfile.o $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $^ $(LIBDIR) $(LIBS)
if_nfile: if_nfile.o $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $^ $(LIBDIR) $(LIBS)
if_dfile: if_dfile.o $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $^ $(LIBDIR) $(LIBS)
if_pfile: if_pfile.o ifile.o $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $^ $(LIBDIR) $(LIBS)
shell: shell.o ifile.o $(OBJECTS)
	$(CC) $(CFLAGS) -lreadline -o $(BINDIR)/$@ $^ $(LIBDIR) $(LIBS)

###------------------------------
### #include dependences 
###------------------------------------------------------------
# you may fill these lines with "make depend"
mkhd.o: mkhd.c 
dmps.o: dmps.c 
frmt.o: frmt.c 
vm.o: vm.c
current.o: current.c 

hw.o: hw.c hw.h
ctx.o: ctx.c ctx.h hw.h sem.h
drive.o: drive.c drive.h
mbr.o: mbr.c drive.h mbr.h
mkvol.o: mbr.c mbr.h drive.h
dvol.o: mbr.c mbr.h drive.h
mknfs.o: mbr.h drive.h superbloc.h ifile.h mknfs.c
dfs.o: mbr.h drive.h superbloc.h dfs.c current.c
superbloc.o: mbr.h drive.h ifile.h superbloc.h  superbloc.c
inode.o:inode.c inode.h mbr.h superbloc.h drive.h
commons.o: commons.c commons.h
builtins.o: builtins.c builtins.h commons.h file.h dir.h mount.h inode.h

if_cfile.o: if_cfile.c ifile.h inode.h tools.h mount.h dir.h file.h
if_dfile.o: if_dfile.c ifile.h inode.h tools.h mount.h dir.h file.h
if_nfile.o: if_nfile.c ifile.h inode.h tools.h mount.h dir.h file.h
if_pfile.o: if_pfile.c ifile.h inode.h tools.h mount.h dir.h file.h
ifile.o: ifile.c inode.h tools.h ifile.h
mount.o: mount.c config.h tools.h inode.h mbr.h superbloc.h drive.h
tools.o: tools.c tools.h
file.o: file.c file.h dir.h ifile.h inode.h tools.h mount.h
dir.o: dir.c dir.h ifile.h inode.h tools.h mount.h
shell.o: shell.c ctx.h mount.h builtins.h commons.h

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

