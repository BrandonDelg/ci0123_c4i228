XC = g++
STD = -std=c++20
WFLAGS = -Wall -Wextra
LIBS = -lssl -lcrypto

BIN = bin

.PHONY: all clean run-serverCC run-clientCC

all: $(BIN)/serverCC.out $(BIN)/clientCC.out

$(BIN):
	mkdir -p $(BIN)

$(BIN)/serverCC.out: SSLServer.cc Socket.cpp VSocket.cpp SSLSocket.cpp | $(BIN)
	$(XC) $(STD) $(WFLAGS) $^ -o $@ $(LIBS)

$(BIN)/clientCC.out: SSLClient.cc Socket.cpp VSocket.cpp SSLSocket.cpp | $(BIN)
	$(XC) $(STD) $(WFLAGS) $^ -o $@ $(LIBS)

run-serverCC: $(BIN)/serverCC.out
	./$(BIN)/serverCC.out $(PORT) $(if $(filter true,$(IPV6)),ipv6)

run-clientCC: $(BIN)/clientCC.out
	./$(BIN)/clientCC.out $(IP) $(PORT) $(if $(filter true,$(IPV6)),ipv6)

clean:
	rm -rf $(BIN)