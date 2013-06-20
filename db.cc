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

void 
DB::EstablishDB() 
{
	m_conn.connect(m_db.c_str(), m_server.c_str(), m_user.c_str(), m_passwd.c_str());
	if(!m_conn.connected()) //connection failed, try to create db first
	{
		cout << "Connection failed. Try to create db." << endl;
		m_conn.connect(0, m_server.c_str(), m_user.c_str(), m_passwd.c_str());
		m_conn.create_db(m_db);
	}
	else
	{
		cout << "Connected successfully." << endl;
	}
}

bool
DB::EstablishTable() 
{
	mysqlpp::Query query = m_conn.query();
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
DB::DBReconnect()
{
	m_conn.disconnect();
	m_conn.connect(m_db.c_str(), m_server.c_str(), m_user.c_str(), m_passwd.c_str());
}
void
DB::Insert(const string & domain, const long latency)
{
	DBReconnect();
	mysqlpp::Query query = m_conn.query();
	query << "INSERT INTO latency VALUES(\'" << domain << "\', " << latency << ");";
	cout << "Query: " << query.str() << endl;
	try{
		mysqlpp::StoreQueryResult result = query.store();
	} catch (const mysqlpp::BadQuery & badqr_ex){
		cerr << "Bad query: " << badqr_ex.what() << endl;
	}
}
