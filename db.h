#define MYSQLPP_MYSQL_HEADERS_BURIED

#include <mysql++/mysql++.h>

#include <string>

using namespace std;

class DB{
public:
	DB(const string & config);
	~DB();
	void Insert(const string & domain, const long latency);
private:
	void Init(const string & config);
	void PrintConfig() const;
	bool EstablishTable();
	void EstablishDB(); 
	void DBReconnect();
private:
	string m_server; //mysql server e.g. localhost
	string m_user; //mysql username
	string m_db; //mysql db
	string m_passwd; //mysql passwd
	mysqlpp::Connection m_conn; //DB Connection
};
