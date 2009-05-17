CC= gcc
CFLAGS=  -g -Wall
SOURCES= tradeSystem.o main.o genData.o

all: trade

trade:  ${SOURCES}
	${CC} -o trade ${CFLAGS} ${SOURCES}

clean:
	rm -f *.o *~ trade
