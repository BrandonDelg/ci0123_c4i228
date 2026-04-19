XC = g++
STD = -std=c++20
WFLAGS = -Wall -Wextra
LIBS = -lssl -lcrypto

BIN = bin

.PHONY: all clean run-serverCC run-clientCC

all: $(BIN)/serverCC.out $(BIN)/clientCC.out

$(BIN):
	mkdir -p $(BIN)

# SERVER
$(BIN)/serverCC.out: SSLServer.cc Socket.cpp VSocket.cpp SSLSocket.cpp | $(BIN)
	$(XC) $(STD) $(WFLAGS) $^ -o $@ $(LIBS)

# CLIENT
$(BIN)/clientCC.out: SSLClient.cc Socket.cpp VSocket.cpp SSLSocket.cpp | $(BIN)
	$(XC) $(STD) $(WFLAGS) $^ -o $@ $(LIBS)

# RUN SERVER
run-serverCC: $(BIN)/serverCC.out
	./$(BIN)/serverCC.out $(PORT)

# RUN CLIENT
run-clientCC: $(BIN)/clientCC.out
	./$(BIN)/clientCC.out $(IP) $(PORT)

clean:
	rm -rf $(BIN)