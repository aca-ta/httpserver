CC=gcc

CFLAGS=-g -Wall -I ./include

MAIN=main/main.o
SIGNAL=signal/signal.o
LOG=log/log.o
MEM=mem/mem.o
DAEMON=daemon/daemon.o
HTTP=					\
	 http/read_req.o	\
	 http/write_res.o	\
	 http/httpcom.o		\
	 http/service.o

OBJGROUP=			\
		 $(MAIN)	\
		 $(SIGNAL)	\
		 $(DAEMON)	\
		 $(LOG)		\
		 $(MEM)		\
		 $(HTTP)	
		

all:program

program: $(OBJGROUP)
	$(CC) -o program $(CFLAGS) $(OBJGROUP) 

clean:
	rm */*.o
	rm program
