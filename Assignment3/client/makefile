CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -lpthread

all: StompClient

StompClient: bin/ConnectionHandler.o bin/StompProtocol.o bin/StompClient.o bin/Event.o bin/Keyboard.o
	g++ -o bin/StompWCIClient bin/ConnectionHandler.o bin/StompProtocol.o bin/StompClient.o bin/Event.o bin/Keyboard.o $(LDFLAGS)

bin/ConnectionHandler.o: src/ConnectionHandler.cpp
	g++ $(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp

bin/echoClient.o: src/echoClient.cpp
	g++ $(CFLAGS) -o bin/echoClient.o src/echoClient.cpp

bin/Event.o: src/Event.cpp
	g++ $(CFLAGS) -o bin/Event.o src/Event.cpp

bin/StompProtocol.o: src/StompProtocol.cpp
	g++ $(CFLAGS) -o bin/StompProtocol.o src/StompProtocol.cpp

bin/StompClient.o: src/StompClient.cpp
	g++ $(CFLAGS) -o bin/StompClient.o src/StompClient.cpp

bin/Keyboard.o: src/Keyboard.cpp
	g++ $(CFLAGS) -o bin/Keyboard.o src/Keyboard.cpp

.PHONY: clean
clean:
	rm -f bin/*



 
	