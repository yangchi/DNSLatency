#include <iostream>
#include <cstring>
#include <string>
#include <cstdlib>
#include "dns.h"

using namespace std;

int main(int argc, char *argv[])
{
	int freq = 10; //default freq of queries is 10ms
	if(argc != 3 || strncmp("--freq", argv[1], 6))
	{
		cout << "Usage: query --freq frequency" << endl
			<< "\tfrequency is in ms"<<endl;
		return 0;
	}
	else
	{
		string freq_str(argv[2]);
		int pos = 0;
		while(freq_str[pos] >= '0' && freq_str[pos] <= '9')
			pos++;
		freq_str = freq_str.substr(0, pos);
		freq = stoi(freq_str);
	}
	cout << "Frequency of queries: " << freq << "ms" << endl;
	DNSQuery dns("domains.in");
	return 0;
}
