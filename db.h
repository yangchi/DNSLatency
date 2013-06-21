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
	vector<double> avgs;
	vector<double> devs;
};

class DB{
public:
	DB(const string & config);
	~DB();
	void Insert(const string & domain, const int64_t latency);
	StatStruct Stats(const vector<string> & domains) const;
private:
	void Init(const string & config);
	void PrintConfig() const;
	bool EstablishTable();
	bool EstablishDB(); 
private:
	string m_server; //mysql server e.g. localhost
	string m_user; //mysql username
	string m_db; //mysql db
	string m_passwd; //mysql passwd
};

#endif
