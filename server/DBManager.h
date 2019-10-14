#pragma once

#include "Predef.h"

#if 0

class DBManager
{
public :
	static DBManager * getInstance()
	{
		if (_instance == nullptr)
		{
			_instance = new DBManager();
		}
		return _instance;
	}

	bool init(const std::string& host, int port=6379);
	bool authRedis(const std::string& pw);
	bool sendCommand(const std::string& command);

	Poco::Redis::BulkString getFromHashTBL(const std::string& tbl, const std::string& key);

private :
	Poco::Redis::Client client;

private :
	DBManager();
	~DBManager();

	static DBManager *_instance;
	bool	isconnected;
};
#endif