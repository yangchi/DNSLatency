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
	EstablishStatsTable();
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
	catch(const mysqlpp::Exception & ex) 
	{
		cerr << "Error: " << ex.what() << endl;
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
	}
	catch (const mysqlpp::BadQuery & badqr_ex)
	{ //latency table not exists. create one
		query.reset();
		query << "CREATE TABLE latency (domains VARCHAR(20) NOT NULL, latency BIGINT NOT NULL)";
		query.execute();
		cout << "Created Table latency." << endl;
		return true;
	}
	catch(const mysqlpp::Exception & ex) 
	{
		cerr << "Error: " << ex.what() << endl;
		return false;
	}
}

bool
DB::EstablishStatsTable()
{
	mysqlpp::Connection conn(m_db.c_str(), m_server.c_str(), m_user.c_str(), m_passwd.c_str());
	mysqlpp::Query query = conn.query();
	query << "select 1 from stats"; //check if stats table already exists from previous running
	try{
		query.execute();
		cout << "Table stats already exists." << endl;
		return true;
	}
	catch (const mysqlpp::BadQuery & badqr_ex)
	{ //stats table not exists. create one
		query.reset();
		query << "CREATE TABLE stats (domains VARCHAR(20) NOT NULL, mean DOUBLE DEFAULT 0, dev DOUBLE DEFAULT 0, count INT DEFAULT 0, first TIMESTAMP NULL, last TIMESTAMP NULL, PRIMARY KEY (`domains`) )";
		query.execute();
		cout << "Created Table stats." << endl;
		return true;
	}
	catch(const mysqlpp::Exception & ex) 
	{
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
	try{
		mysqlpp::StoreQueryResult result = query.store();
	}
	catch (const mysqlpp::BadQuery & badqr_ex)
	{
		cerr << "Bad query: " << badqr_ex.what() << endl;
	}
	catch(const mysqlpp::Exception & ex) 
	{
		cerr << "Error: " << ex.what() << endl;
	}
	UpdateStats(domain);
}

void
DB::UpdateTime(const string & domain)
{
	mysqlpp::Connection conn(m_db.c_str(), m_server.c_str(), m_user.c_str(), m_passwd.c_str());
	mysqlpp::Query query = conn.query();
	/*
	 * The following SQL update both the first query time and the latest query time with current time stamp
	 * But when the domain already exists in the table, only the latest query time gets updated
	 * So the first query time will only be inserted once.
	 * Note that UpdateTime() function will be called before UpdateStats() function, so the first query time won't be NULL
	 */
	query << "INSERT INTO stats (domains, first, last) values (" << mysqlpp::quote_only << domain << ", CURRENT_TIMESTAMP, CURRENT_TIMESTAMP) ON DUPLICATE KEY UPDATE last=CURRENT_TIMESTAMP";
	try{
		query.execute();
	}
	catch(const mysqlpp::BadQuery & badqr_ex)
	{
		cerr << "Failed to update latest timestamp of query to domain: " << domain << endl;
		cerr << badqr_ex.what() << endl;
	}
	catch(const mysqlpp::Exception & ex) 
	{
		cerr << "Error: " << ex.what() << endl;
	}
}

void
DB::UpdateStats(const string & domain)
{
	mysqlpp::Connection conn(m_db.c_str(), m_server.c_str(), m_user.c_str(), m_passwd.c_str());
	mysqlpp::Query query = conn.query();
	query << "insert into stats (domains, count, mean, dev) SELECT " 
		<< mysqlpp::quote_only 
		<< domain 
		<< ", COUNT(latency), AVG(latency), STDDEV_POP(latency) from latency where latency.domains=" 
		<< mysqlpp::quote_only 
		<< domain 
		<< " ON DUPLICATE KEY UPDATE count=values(count), mean = values(mean), dev = values(dev)";
	try{
		query.execute();
	}
	catch (const mysqlpp::BadQuery & badqr_ex)
	{
		cerr << "Failed to update stats: Bad Query: " << badqr_ex.what() << endl;
	}
	catch(const mysqlpp::Exception & ex) 
	{
		cerr << "Error: " << ex.what() << endl;
	}

}

StatStruct
DB::Stats(const vector<string> & domains) const
{
	StatStruct stats;
	mysqlpp::Connection conn(m_db.c_str(), m_server.c_str(), m_user.c_str(), m_passwd.c_str());
	vector<string>::const_iterator iter;
	for(iter = domains.begin(); iter != domains.end(); iter++)
	{
		mysqlpp::Query query = conn.query();
		query << "SELECT count, mean, dev, first, last FROM stats where domains = " << mysqlpp::quote_only << (*iter);
		mysqlpp::StoreQueryResult result = query.store();
		try{
			stats.counts.push_back(result[0][0]);
			stats.avgs.push_back(result[0][1]);
			stats.devs.push_back(result[0][2]);
			stats.first.push_back(string(result[0][3]));
			stats.last.push_back(string(result[0][4]));
		}
		catch(const mysqlpp::BadConversion & badconv_ex)
		{
			cerr << "Bad Conversion: " << badconv_ex.what() << endl;
			stats.counts.push_back(-1);
			stats.avgs.push_back(-1);
			stats.devs.push_back(-1);
			stats.first.push_back("Bad Conversion");
			stats.last.push_back("Bad Conversion");
		}
		catch(const mysqlpp::Exception & ex) 
		{
			cerr << "Error: " << ex.what() << endl;
			stats.counts.push_back(-1);
			stats.avgs.push_back(-1);
			stats.devs.push_back(-1);
			stats.first.push_back("Bad Conversion");
			stats.last.push_back("Bad Conversion");
		}
	}
	return stats;
}
