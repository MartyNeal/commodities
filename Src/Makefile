CC= gcc
CFLAGS= -Wall -g -O0 -lm
SOURCES= tradeSystem.c main.c genData.c norm.c sim.c randomAlgorithm.c ga.c learner.c

all: trade

trade:  ${SOURCES}
	${CC} -o trade ${CFLAGS} ${SOURCES}

#debug:  ${SOURCES}
#	${CC} -o trade -g ${CFLAGS} ${SOURCES}

ptrade: ${SOURCES}
	mpicc -Wall -o trade ${SOURCES} -lm

tags:
	rm TAGS; etags *.c *.h

clean:
	rm -f *.o *~ trade
