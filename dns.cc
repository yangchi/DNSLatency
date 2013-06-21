#include "dns.h"
#include <cstdint>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include <ctime>
#include "sys/time.h"

DNSQuery::DNSQuery(const string & domainfile, const string & dbconfig)
	: m_config(domainfile), m_db(dbconfig)
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
	cout << "Domains from config file: " << endl;
	vector<string>::const_iterator iter;
	for(iter = m_domains.begin(); iter != m_domains.end(); iter++)
		cout << "\t" << (*iter) << endl;
}

bool
DNSQuery::Init()
{
	InitDomains();
	PrintDomains();
	return true;
}

bool
DNSQuery::Query()
{
	ldns_resolver *resolver;
	ldns_pkt *pkt;
	ldns_status status = ldns_resolver_new_frm_file(&resolver, NULL);	
	if(status != LDNS_STATUS_OK)
		return false;
	vector<string>::const_iterator iter;
	for(iter = m_domains.begin(); iter != m_domains.end(); iter++)
	{
		string rand_domain = RandomString(7) + "." + (*iter);
		cout << "Randomized: " << rand_domain << endl;
		ldns_rdf *domain = ldns_dname_new_frm_str(rand_domain.c_str());
		if(!domain)
		{
			cerr << "\tcould not create the rdf obj" << endl;
			return false;
		}
		struct timespec tspec_start, tspec_end;
		clock_gettime(CLOCK_MONOTONIC, &tspec_start); //get time before the query
		pkt = ldns_resolver_query(resolver, domain, LDNS_RR_TYPE_A, LDNS_RR_CLASS_IN, LDNS_RD);
		clock_gettime(CLOCK_MONOTONIC, &tspec_end);
		int64_t elapsed_nanos = 1000000000 * (tspec_end.tv_sec - tspec_start.tv_sec) + tspec_end.tv_nsec - tspec_start.tv_nsec; //DNS query latency
		cout << "\tTime elapsed: " << elapsed_nanos << " nanoseconds" << endl;
		m_db.Insert(*iter, elapsed_nanos); //store it to the db
		ldns_rdf_deep_free(domain);
		if(!pkt)
		{
			cerr << "\tnameserver could not resolve the domain" << endl;
			return false;
		}
		ldns_rr_list *result = ldns_pkt_rr_list_by_type(pkt, LDNS_RR_TYPE_A, LDNS_SECTION_ANSWER);
		if(!result)
		{
			cout << "\tnameserver resolved but could not find this domain" << endl;
		}
		else
		{
			ldns_rr_list_sort(result);
			ldns_rr_list_print(stdout, result);
			ldns_rr_list_deep_free(result);
		}
		ldns_pkt_free(pkt);
	}
	ldns_resolver_deep_free(resolver);
	return true;
}

string
RandomString(const int length)
{
	static const string alphanumeric = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	struct timeval curtime;
	gettimeofday(&curtime, NULL);
	srand(curtime.tv_usec);
	string result;
	for(int i = 0; i < length; i++)
	{
		result.push_back(alphanumeric[rand() % (10+26+26)]);
	}
	return result;
}
