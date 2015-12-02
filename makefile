CXX=g++
CXXFLAGS=-c -std=c++11 -Wall

all: atm proxy bank

atm: atm.o tcp_client.o
	$(CXX) atm.o tcp_client.o -o atm

proxy: proxy.o tcp_client.o
	$(CXX) proxy.o tcp_client.o -o proxy

bank: bank.o server.o
	$(CXX) -pthread bank.o server.o -o bank -lcrypto++

atm.o: atm.cpp
	$(CXX) $(CXXFLAGS) atm.cpp

proxy.o: proxy.cpp
	$(CXX) $(CXXFLAGS) proxy.cpp

bank.o: bank.cpp
	$(CXX) $(CXXFLAGS) bank.cpp

tcp_client.o: tcp_client.cpp
	$(CXX) $(CXXFLAGS) tcp_client.cpp

server.o: server.cpp
	$(CXX) $(CXXFLAGS) server.cpp

clean:
	rm *.o