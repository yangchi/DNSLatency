all: dns.o query.o
	g++ query.o dns.o -o query -std=c++11 -lldns -g
query.o: query.cc
	g++ -std=c++11 -c query.cc -g
dns.o: dns.cc
	g++ -c dns.cc -g
clean:
	rm -rf query *.o
