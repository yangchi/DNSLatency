#include <ldns/ldns.h>
#include <string>
#include <vector>

using namespace std;

string RandomString(const int length);

class DNSQuery{
public:
	DNSQuery();
	DNSQuery(const string & config);
	~DNSQuery();
	bool Query();
private:
	bool Init();
	void InitDomains();
	void PrintDomains() const;
private:
	string m_config; //domains config filename
	vector<string> m_domains;
};
