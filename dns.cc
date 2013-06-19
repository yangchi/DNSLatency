#include "dns.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>

DNSQuery::DNSQuery()
{}

DNSQuery::DNSQuery(const string & config)
	: m_config(config)
{
	Init();
}

DNSQuery::~DNSQuery()
{}

void
DNSQuery::InitDomains()
{
	ifstream domainfile(m_config.c_str());
	copy(istream_iterator<string>(domainfile), istream_iterator<string>(), back_inserter(m_domains));
}

void
DNSQuery::PrintDomains() const
{
	cout << "Domains to query: " << endl;
	vector<string>::const_iterator iter;
	for(iter = m_domains.begin(); iter != m_domains.end(); iter++)
		cout << "\t" << (*iter) << endl;
}

bool
DNSQuery::Init()
{
	InitDomains();
	PrintDomains();
	ldns_status status = ldns_resolver_new_frm_file(&m_resolver, NULL);	
	if(status != LDNS_STATUS_OK)
		return false;
	return true;
}
