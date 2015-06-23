
CC=g++ -std=c++11
CXXFLAGS = -g -Wall -Werror -ansi -pedantic -I/usr/include/Poco -I/usr/include/Poco/Net -I/usr/local/Poco/UtilL -I/usr/local/Poco/Thread
CF=
LDFLAGS= $(CXXFLAGS) -lPocoFoundationd -lPocoNetd -lPocoUtild 



all: poco

poco: poco.o
	$(CC) -o poco poco.o  $(LDFLAGS)

poco.o: poco.cc
	$(CC) -o poco.o -c poco.cc $(CF)
