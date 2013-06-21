#include <iostream>
#include <cstring>
#include <string>
#include <cstdlib>
#include <cstdint>
#include "dns.h"
#include "db.h"
#include "unistd.h"

using namespace std;

int main(int argc, char *argv[])
{
	int64_t freq = 1000000; //default freq of queries is 1s
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
		freq = stol(freq_str);
	}
	cout << "Frequency of queries: " << freq << "ms" << endl;
	DNSQuery dns("domains.in", "dbconfig.in");
	int loop = 10;
	while(loop--)
	{
		dns.Query();
		sleep(freq/1000000);
		usleep(freq%1000000);
	}
	//DB db("dbconfig.in");
	//db.Insert("google.com", 123456789);
	//db.Insert("google.com", 9876543210);
	return 0;
}
