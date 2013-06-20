using namespace std;

class DB{
public:
	DB(const string & config);
	~DB();
	void EstablishTable() const;
private:
	void Init(const string & config);
	void PrintConfig() const;
private:
	string m_server; //mysql server e.g. localhost
	string m_user; //mysql username
	string m_db; //mysql db
	string m_passwd; //mysql passwd
};
