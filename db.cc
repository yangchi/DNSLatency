#define MYSQLPP_MYSQL_HEADERS_BURIED

#include <mysql++/mysql++.h>

#include "db.h"
#include <fstream>
#include <string>

DB::DB(const string & config)
{
	Init(config);
	PrintConfig();
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
}

void
DB::EstablishTable() const
{
}

void
DB::PrintConfig() const
{
	cout << "Server: " << m_server << endl
		<< "Username: " << m_user << endl
		<< "Password: " << m_passwd << endl
		<< "Database: " << m_db << endl;
}
