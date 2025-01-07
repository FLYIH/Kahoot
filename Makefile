include ../Make.defines

CXX = g++
CXXFLAGS = -Wall -g
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system
PROGS =	server client_gui test_client

all:	${PROGS}

server:	server.o
		${CC} ${CFLAGS} -o $@ server.o ${LIBS}

test_client:	client/test_client.o
		${CC} ${CFLAGS} -o $@ client/test_client.o ${LIBS}

client_gui:	client/client.o client/sfml_gui.o client/MainMenu.o client/WaitingScreen.o client/QuizScreen.o client/RankingScreen.o client/ResultScreen.o client/FinalScreen.o client/main.o
		${CXX} ${CXXFLAGS} -I../lib -o $@ client/client.o client/sfml_gui.o client/MainMenu.o client/WaitingScreen.o client/QuizScreen.o client/RankingScreen.o client/ResultScreen.o client/FinalScreen.o client/main.o ${LIBS} ${SFML_LIBS}

client/client.o:	client/client.c
		${CC} ${CFLAGS} -c client/client.c -o client/client.o

client/FinalScreen.o:	client/FinalScreen.cpp
		${CXX} ${CXXFLAGS} -I../lib -c client/FinalScreen.cpp -o client/FinalScreen.o

client/ResultScreen.o:	client/ResultScreen.cpp
		${CXX} ${CXXFLAGS} -I../lib -c client/ResultScreen.cpp -o client/ResultScreen.o

client/RankingScreen.o:	client/RankingScreen.cpp
		${CXX} ${CXXFLAGS} -I../lib -c client/RankingScreen.cpp -o client/RankingScreen.o

client/QuizScreen.o:	client/QuizScreen.cpp
		${CXX} ${CXXFLAGS} -I../lib -c client/QuizScreen.cpp -o client/QuizScreen.o

client/WaitingScreen.o:	client/WaitingScreen.cpp
		${CXX} ${CXXFLAGS} -I../lib -c client/WaitingScreen.cpp -o client/WaitingScreen.o

client/sfml_gui.o:	client/sfml_gui.cpp
		${CXX} ${CXXFLAGS} -I../lib -c client/sfml_gui.cpp -o client/sfml_gui.o

client/MainMenu.o:	client/MainMenu.cpp
		${CXX} ${CXXFLAGS} -I../lib -c client/MainMenu.cpp -o client/MainMenu.o

client/main.o:	client/main.cpp
		${CXX} ${CXXFLAGS} -I../lib -c client/main.cpp -o client/main.o

clean:
		rm -f ${PROGS} *.o client/*.o
