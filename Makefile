#CC = /usr/bin/c++
CC = g++

# use these compile flags
CFLAGS = -c

# use this debug flag
#DFLAGS = -g
DFLAGS = 

# use this optimization flag
OFLAGS = -O3
#OFLAGS = 

# use these include directories
INCLUDE_DIRS = -I.

# use all the flags
AFLAGS = $(CFLAGS) $(DFLAGS) $(OFLAGS)

CHESS_OBJS = chngdpth.o  \
             chngmxtm.o  \
             dispmenu.o  \
             docomand.o  \
             evaluate.o  \
             evaluate1.o \
             evaluate2.o \
             givmeval.o  \
             glob.o      \
             gncmpmov.o  \
             hashbrd.o   \
             hashtabl.o  \
             initial.o   \
             io.o        \
             killer.o    \
             letusrmv.o  \
             main.o      \
             openbook.o  \
             piececod.o  \
             play.o      \
             position.o  \
             statval.o   \
             updmvlst.o  \
             utility.o

pawnder : $(CHESS_OBJS)
	@echo "Linking pawnder.  `date`"
	$(CC) $(CHESS_OBJS) $(DFLAGS) -o pawnder

clean :
	rm *.o

verify :
	./scripts/regr.scr

.c.o : 
	@echo "Compiling $*.c  `date`"
	$(CC) $(AFLAGS) $(INCLUDE_DIRS) $*.c

chngdpth.o : glob.h chngdpth.h chngdpth.c
chngmxtm.o : glob.h chngmxtm.h chngmxtm.c
dispmenu.o : glob.h chngdpth.h chngmxtm.h hashbrd.h initial.h play.h position.h dispmenu.h dispmenu.c
docomand.o : glob.h chngdpth.h chngmxtm.h hashbrd.h initial.h position.h utility.h docomand.h docomand.c
evaluate.o : glob.h hashbrd.h position.h statval.h utility.h evaluate.h evaluate.c
evaluate1.o : glob.h hashbrd.h position.h statval.h utility.h evaluate1.h evaluate1.c
evaluate2.o : glob.h hashbrd.h position.h statval.h utility.h evaluate2.h evaluate2.c
givmeval.o : glob.h debug.h evaluate.h evaluate1.h evaluate2.h hashbrd.h hashtabl.h killer.h position.h utility.h givmeval.h givmeval.c
glob.o     : glob.h debug.h hashbrd.h hashtabl.h killer.h openbook.h position.h glob.c
gncmpmov.o : glob.h debug.h givmeval.h hashbrd.h hashtabl.h io.h killer.h openbook.h position.h updmvlst.h utility.h gncmpmov.h gncmpmov.c
hashbrd.o  : glob.h hashbrd.h hashtabl.h position.h utility.h hashbrd.c
hashtabl.o : glob.h debug.h hashbrd.h hashtabl.h position.h hashtabl.c
initial.o  : glob.h debug.h hashbrd.h position.h initial.h initial.c
io.o       : glob.h hashbrd.h position.h utility.h io.h io.c
killer.o   : glob.h killer.h killer.c
letusrmv.o : glob.h docomand.h hashbrd.h io.h position.h updmvlst.h utility.h letusrmv.h letusrmv.c
main.o     : glob.h dispmenu.h initial.h main.c
openbook.o : glob.h debug.h hashbrd.h io.h position.h utility.h openbook.h openbook.c
piececod.o : glob.h piececod.h piececod.c
play.o     : glob.h gncmpmov.h hashbrd.h hashtabl.h io.h letusrmv.h position.h utility.h play.h play.c
position.o : glob.h debug.h hashbrd.h killer.h piececod.h statval.h utility.h position.h position.c
statval.o  : glob.h statval.h statval.c
updmvlst.o : glob.h debug.h hashbrd.h io.h position.h utility.h updmvlst.h updmvlst.c
utility.o  : glob.h hashbrd.h position.h statval.h utility.h utility.c


