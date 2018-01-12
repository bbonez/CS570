# makefile for Program Two
# SDSU CS570

PROGRAM = p2
CC = gcc
CCFLAGS = -g -Wall

${PROGRAM}: getword.o p2.o
			${CC} ${CCFLAGS} -o p2 getword.o p2.o

p2.o:		getword.h

getword.o: 	getword.h

clean:
			rm -f *.o ${PROGRAM} your.output*

splint:
			splint -warnposix +trytorecover -weak getword.c ${PROGRAM}.c

