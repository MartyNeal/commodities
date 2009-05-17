#  $Id: Makefile,v 1.7 2009/03/07 00:49:42 engleyr Exp $

#make file for CS352, W09 assignment 3

CC= gcc
CFLAGS=  -g -Wall
SOURCES= tradeSystem.o main.o

all: trade

trade:	${SOURCES}	
	${CC} -o trade ${CFLAGS} ${SOURCES} 

clean:
	rm -f *.o *~ trade


