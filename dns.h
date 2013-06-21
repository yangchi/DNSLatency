#ifndef __DNS_H_
#define __DNS_H_

#include <ldns/ldns.h>
#include <string>
#include <vector>

#include "db.h"

using namespace std;

string RandomString(const int length);

class DNSQuery{
public:
	DNSQuery(const string & domainfile, const string & dbconfig);
	~DNSQuery();
	bool Query();
private:
	bool Init();
	void InitDomains();
	void PrintDomains() const;
private:
	string m_config; //domains config filename
	vector<string> m_domains;
	DB m_db; //db operations - defined in db.h db.cc
};

#endif
