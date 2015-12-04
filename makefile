CXX=g++
CXXFLAGS=-c -std=c++11 -Wall

all: atm proxy bank

atm: atm2.o crypto.o
	$(CXX) atm2.o crypto.o -o atm -lcryptopp

proxy: proxy2.o
	$(CXX) -pthread proxy2.o -o proxy

bank: bank2.o crypto.o
	$(CXX) -pthread bank2.o crypto.o -o bank -lcryptopp

atm2.o: atm2.cpp
	$(CXX) $(CXXFLAGS) atm2.cpp

proxy2.o: proxy2.cpp
	$(CXX) $(CXXFLAGS) proxy2.cpp

bank2.o: bank2.cpp
	$(CXX) $(CXXFLAGS) bank2.cpp

crypto.o: crypto.cpp
	$(CXX) $(CXXFLAGS) crypto.cpp

#tcp_client.o: tcp_client.cpp
#	$(CXX) $(CXXFLAGS) tcp_client.cpp

#server.o: server.cpp
#	$(CXX) $(CXXFLAGS) server.cpp

clean:
	rm *.o