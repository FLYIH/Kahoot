include ../Make.defines

CXX = g++
CXXFLAGS = -Wall -g
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system
PROGS =	server client_gui

all:	${PROGS}
		
server:	server.o
		${CC} ${CFLAGS} -o $@ server.o ${LIBS}

client_gui:	client.o sfml_gui.o MainMenu.o main.o
		${CXX} ${CXXFLAGS} -I../lib -o $@ client.o sfml_gui.o MainMenu.o main.o ${LIBS} ${SFML_LIBS}

client.o:	client.c
		${CC} ${CFLAGS} -c client.c -o client.o

sfml_gui.o:	sfml_gui.cpp
		${CXX} ${CXXFLAGS} -I../lib -c sfml_gui.cpp -o sfml_gui.o

MainMenu.o:	MainMenu.cpp
		${CXX} ${CXXFLAGS} -I../lib -c MainMenu.cpp -o MainMenu.o

main.o:	main.cpp
		${CXX} ${CXXFLAGS} -I../lib -c main.cpp -o main.o

clean:
		rm -f ${PROGS} *.o
