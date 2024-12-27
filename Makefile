CXX = g++
CXXFLAGS = -Wall -g
LIBS = -lsfml-graphics -lsfml-window -lsfml-system

SRC = src
TARGET = kahoot
OBJS = $(SRC)/main.o $(SRC)/MainMenu.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

$(SRC)/%.o: $(SRC)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
