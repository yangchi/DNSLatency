#include <ldns/ldns.h>
#include <string>
#include <vector>

using namespace std;

class DNSQuery{
public:
	DNSQuery();
	DNSQuery(const string & config);
	~DNSQuery();
private:
	bool Init();
	void InitDomains();
	void PrintDomains() const;
private:
	ldns_resolver *m_resolver;
	ldns_pkt *m_pkt;
	string m_config; //domains config filename
	vector<string> m_domains;
};
