SRC = ./src
OBJ = ./obj
INC = ./inc

INCLUDES += -I./inc

CXXFLAGS = -pthread

TARGETS = client server

OBJ_LIB_CLIENT = $(OBJ)/client.o $(OBJ)/tcpsocket.o
OBJ_LIB_SERVER = $(OBJ)/server.o $(OBJ)/tcpsocket.o $(OBJ)/tcpserver.o $(OBJ)/ThreadPool.o

all: $(TARGETS)

client:$(OBJ_LIB_CLIENT)
	g++ $^ $(CXXFLAGS) -o $@ 

server:$(OBJ_LIB_SERVER)
	g++ $^ $(CXXFLAGS) -o $@

$(OBJ)/%.o: $(SRC)/%.cpp $(INC)/%.h
	g++ -c $< $(CXXFLAGS) -o $@ $(INCLUDES)

$(OBJ)/%.o: $(SRC)/%.cpp
	g++ -c $< $(CXXFLAGS) -o $@ $(INCLUDES)

clean:
	rm -rf $(OBJ)/*

