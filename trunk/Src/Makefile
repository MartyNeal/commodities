CC= gcc
CFLAGS= -Wall -g -O0
SOURCES= tradeSystem.c main.c genData.c

all: trade

trade:  ${SOURCES}
	${CC} -o trade ${CFLAGS} ${SOURCES}

#debug:  ${SOURCES}
#	${CC} -o trade -g ${CFLAGS} ${SOURCES}

clean:
	rm -f *.o *~ trade
