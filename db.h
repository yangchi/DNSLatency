#ifndef __DB_H_
#define __DB_H_

#define MYSQLPP_MYSQL_HEADERS_BURIED

#include <mysql++/mysql++.h>

#include <cstdint>
#include <string>
#include <vector>

using namespace std;

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
	void Insert(const string & domain, const int64_t latency);
	StatStruct Stats(const vector<string> & domains) const;
	void UpdateStats(const string & domain);
	void UpdateTime(const string & domain);//update the timestamp of both first and latest query
private:
	void Init(const string & config);
	void PrintConfig() const;
	bool EstablishTable();
	bool EstablishStatsTable();
	bool EstablishDB(); 
private:
	string m_server; //mysql server e.g. localhost
	string m_user; //mysql username
	string m_db; //mysql db
	string m_passwd; //mysql passwd
};

#endif
