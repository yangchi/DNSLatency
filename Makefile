all: dns.o db.o query.o
	g++ query.o dns.o db.o -o query -std=c++11 -lldns -g
query.o: query.cc
	g++ -std=c++11 -c query.cc -g
db.o: db.cc
	g++ -c db.cc -g 
dns.o: dns.cc
	g++ -c dns.cc -g
clean:
	rm -rf query *.o
