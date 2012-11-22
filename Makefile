CC	=	gcc
SMP	:=	$(shell grep ^SMP /usr/src/linux/Makefile)
CFLAGS	=	-D__KERNEL__ -Wall -O2 -DMODULE -I/usr/src/linux/include
ifdef SMP
CFLAGS	+=	-D__SMP__
endif

all:	check_dep changer.o

clean:
	-rm *.o *~ .depend

dep:
	@echo Calculating changer.c dependences...
	@>.depend;					\
	$(CC) $(CFLAGS) -M changer.c >>.depend;

ifneq ($(wildcard .depend),)
check_dep:
include .depend
else
check_dep: dep
endif

