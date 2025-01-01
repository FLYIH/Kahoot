include ../Make.defines

CXX = g++
CXXFLAGS = -Wall -g
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system
PROGS =	server client_gui test_client uploadserver uploadclient

all:	${PROGS}

uploadserver:	uploadserver.o
		${CC} ${CFLAGS} -o $@ uploadserver.o ${LIBS}

# uploadclient:	uploadclient.o
# 		${CC} -I../lib -g -O2 -D_REENTRANT -Wall -o $@ uploadclient.o ${LIBS} -lncurses

uploadclient: uploadclient.o
	g++ -I../lib -g -O2 -D_REENTRANT -Wall -o uploadclient uploadclient.o -I/opt/homebrew/opt/ncurses/include -L/opt/homebrew/opt/ncurses/lib -lncurses


server:	server.o
		${CC} ${CFLAGS} -o $@ server.o ${LIBS}

test_client:	test_client.o
		${CC} ${CFLAGS} -o $@ test_client.o ${LIBS}

client_gui:	client.o sfml_gui.o MainMenu.o WaitingScreen.o QuizScreen.o RankingScreen.o ResultScreen.o main.o
		${CXX} ${CXXFLAGS} -I../lib -o $@ client.o sfml_gui.o MainMenu.o WaitingScreen.o QuizScreen.o RankingScreen.o ResultScreen.o main.o ${LIBS} ${SFML_LIBS}

client.o:	client.c
		${CC} ${CFLAGS} -c client.c -o client.o

ResultScreen.o:	ResultScreen.cpp
		${CXX} ${CXXFLAGS} -I../lib -c ResultScreen.cpp -o ResultScreen.o

RankingScreen.o:	RankingScreen.cpp
		${CXX} ${CXXFLAGS} -I../lib -c RankingScreen.cpp -o RankingScreen.o

QuizScreen.o:	QuizScreen.cpp
		${CXX} ${CXXFLAGS} -I../lib -c QuizScreen.cpp -o QuizScreen.o

WaitingScreen.o:	WaitingScreen.cpp
		${CXX} ${CXXFLAGS} -I../lib -c WaitingScreen.cpp -o WaitingScreen.o

sfml_gui.o:	sfml_gui.cpp
		${CXX} ${CXXFLAGS} -I../lib -c sfml_gui.cpp -o sfml_gui.o

MainMenu.o:	MainMenu.cpp
		${CXX} ${CXXFLAGS} -I../lib -c MainMenu.cpp -o MainMenu.o

main.o:	main.cpp
		${CXX} ${CXXFLAGS} -I../lib -c main.cpp -o main.o

clean:
		rm -f ${PROGS} *.o