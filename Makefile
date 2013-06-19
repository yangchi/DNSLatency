all: dns.o query.o
	g++ query.o dns.o -o query -std=c++11
query.o: query.cc
	g++ -std=c++11 -c query.cc
dns.o: dns.cc
	g++ -c dns.cc
clean:
	rm -rf query *.o
