#include "db.h"
#include <fstream>

DB::DB(const string & config)
{
	Init(config);
}

DB::~DB(){}

void
DB::Init(const string & config)
{
	ifstream config_file(config.c_str());
	if(config_file.is_open())
	{
		while(config_file.good())
		{
			string line;
			getline(config_file, line);
			if(line.find("server") != line.npos)
			{
				m_server = line.substr(line.find(":")+1);
			}
			if(line.find("username") != line.npos)
			{
				m_user= line.substr(line.find(":")+1);
			}if(line.find("db") != line.npos)
			{
				m_db= line.substr(line.find(":")+1);
			}if(line.find("password") != line.npos)
			{
				m_passwd= line.substr(line.find(":")+1);
			}
		}
		config_file.close();
	}
	PrintConfig();
	EstablishDB();
	EstablishTable();
}

bool
DB::EstablishDB() 
{
	try{
		mysqlpp::Connection conn("information_schema", m_server.c_str(), m_user.c_str(), m_passwd.c_str());
		cout << "Connected to the information_schema db successfully." << endl;
		mysqlpp::Query query = conn.query();
		query << "SELECT * FROM SCHEMATA WHERE SCHEMA_NAME = \'" << m_db.c_str() << "\'";
		mysqlpp::StoreQueryResult result = query.store();
		if(!result.size())
		{
			conn.create_db(m_db.c_str());
		}
		return true;
	}
	catch(const mysqlpp::ConnectionFailed & connfail_ex)
	{
		cerr << "Connection to the information_schema db failed." << endl;
		return false;
	}
	catch(const mysqlpp::BadQuery & badqr_ex)
	{
		cerr << "Bad Query: " << badqr_ex.what() << endl;
		return false;
	}
}

bool
DB::EstablishTable() 
{
	mysqlpp::Connection conn(m_db.c_str(), m_server.c_str(), m_user.c_str(), m_passwd.c_str());
	mysqlpp::Query query = conn.query();
	query << "select 1 from latency"; //check if latency table already exists from previous running
	try{
		query.execute();
		cout << "Table latency already exists." << endl;
		return true;
	} catch (const mysqlpp::BadQuery & badqr_ex){ //latency table not exists. create one
		query.reset();
		query << "CREATE TABLE latency (domains VARCHAR(20) NOT NULL, latency BIGINT NOT NULL)";
		query.execute();
		cout << "Created Table latency." << endl;
		return true;
	} catch(const mysqlpp::Exception & ex) {
		cerr << "Error: " << ex.what() << endl;
		return false;
	}
}

void
DB::PrintConfig() const
{
	cout << "Server: " << m_server << endl
		<< "Username: " << m_user << endl
		<< "Password: " << m_passwd << endl
		<< "Database: " << m_db << endl;
}

void
DB::Insert(const string & domain, const int64_t latency)
{
	mysqlpp::Connection conn(m_db.c_str(), m_server.c_str(), m_user.c_str(), m_passwd.c_str());
mysqlpp::Query query = conn.query();
	query << "INSERT INTO latency VALUES(\'" << domain << "\', " << latency << ");";
	cout << "Query: " << query.str() << endl;
	try{
		mysqlpp::StoreQueryResult result = query.store();
	} catch (const mysqlpp::BadQuery & badqr_ex){
		cerr << "Bad query: " << badqr_ex.what() << endl;
	}
}
