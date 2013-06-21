#ifndef __DB_H_
#define __DB_H_

#define MYSQLPP_MYSQL_HEADERS_BURIED

#include <mysql++/mysql++.h>

#include <cstdint>
#include <string>
#include <vector>

using namespace std;

/*
 * Stats struct for:
 * counts - number of queries
 * avgs - mean of latencies
 * devs - std dev of latencies
 * first - the earliest query
 * last - the latest query
 */
struct StatStruct
{
	vector<int> counts;
	vector<double> avgs;
	vector<double> devs;
	vector<string> first;
	vector<string> last;
};

class DB{
public:
	DB(const string & config);
	~DB();
	void Insert(const string & domain, const int64_t latency); //insert a latency to the latency table
	StatStruct Stats(const vector<string> & domains) const; //Return stats
	void UpdateStats(const string & domain); //update count, avg and std dev of a domain
	void UpdateTime(const string & domain);//update the timestamp of both first (if first) and latest query
private:
	void Init(const string & config);
	void PrintConfig() const; //print database configuration
	bool EstablishTable(); //create latency table if necessary
	bool EstablishStatsTable(); //create stats table if necessary
	bool EstablishDB();  //create database if necessary
private:
	string m_server; //mysql server e.g. localhost
	string m_user; //mysql username
	string m_db; //mysql db
	string m_passwd; //mysql passwd
};

#endif
