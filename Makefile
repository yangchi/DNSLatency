all: dns.o db.o query.o
	g++ query.o dns.o db.o -o query -std=c++11 -lldns -g -lmysqlpp
query.o: query.cc
	g++ -std=c++11 -c query.cc -g
db.o: db.cc
	g++ -c db.cc -g -std=c++11
dns.o: dns.cc
	g++ -c dns.cc -g -std=c++11
clean:
	rm -rf query *.o
