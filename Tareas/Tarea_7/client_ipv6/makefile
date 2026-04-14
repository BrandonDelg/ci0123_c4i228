CXX = g++
STD = -std=c++20
WFLAGS = -Wall -Wextra
CXXFLAGS = $(STD) $(WFLAGS) -g -pthread

SRC = src
BUILD = build
BIN = bin

SERVER = $(BIN)/server.out
CLIENT = $(BIN)/client.out

DIRS = $(shell find -L $(SRC) -type d)
FSOURCE = $(wildcard $(DIRS:%=%/*.cpp))

SERVER_SRC = $(filter %ThreadMirrorServer.cpp %FileSystem.cpp %Socket.cpp %VSocket.cpp, $(FSOURCE))
CLIENT_SRC = $(filter %main.cpp %Client.cpp %Logger.cpp %Socket.cpp %VSocket.cpp, $(FSOURCE))

SERVER_OBJ = $(SERVER_SRC:$(SRC)/%.cpp=$(BUILD)/%.o)
CLIENT_OBJ = $(CLIENT_SRC:$(SRC)/%.cpp=$(BUILD)/%.o)

.PHONY: all server client run-server run-client clean

all: server client

server: $(SERVER)

$(SERVER): $(SERVER_OBJ) | $(BIN)
	$(CXX) $(CXXFLAGS) $^ -o $@

client: $(CLIENT)

$(CLIENT): $(CLIENT_OBJ) | $(BIN)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILD)/%.o: $(SRC)/%.cpp | $(BUILD)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

$(BIN):
	mkdir -p $(BIN)

run-server:
	./$(SERVER)

run-client:
	./$(CLIENT)

clean:
	rm -rf $(BUILD) $(BIN)