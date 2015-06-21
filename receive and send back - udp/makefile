
CC=g++ -std=c++11
CXXFLAGS = -g -Wall -Werror -ansi -pedantic -I/usr/include/Poco -I/usr/include/Poco/Net -I/usr/local/Poco/Util -I/usr/local/Poco/XML
CF=
LDFLAGS= $(CXXFLAGS) -lPocoFoundationd -lPocoNetd -lPocoUtild -lPocoXMLd 



all: poco

poco: poco.o
	$(CC) -o poco poco.o  $(LDFLAGS)

poco.o: poco.cc
	$(CC) -o poco.o -c poco.cc $(CF)
