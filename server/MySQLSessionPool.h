#ifndef MYSQL_SESSION_POOL
#define MYSQL_SESSION_POOL

#include "Predef.h"


class MySQLSessionPool
{
private:
	MySQLSessionPool();
	~MySQLSessionPool();

	static MySQLSessionPool *_instance;
	SessionPool	*mysqlpool;

	int		maxSession;
	int		idleTime;

public:
	bool	init(const std::string &connector, const std::string& connectionString, int minSessions = 1, int maxSessions = 32, int idleTime = 60);
	void	uninit();

	static MySQLSessionPool *getInstance()
	{
		if (_instance == nullptr)
			_instance = new MySQLSessionPool();

		return _instance;
	}

	SessionPool	*getSessionPool();

	void	dbInfo();
	void	query(const std::string &query_str);
	void	query(const std::string &query_str, Poco::Any &p1);
	void	query(const std::string &query_str, Poco::Any &p1, Poco::Any &p2);

};



#endif
