#pragma once

#include "Predef.h"


class RedisManager
{
public :
	static RedisManager * getInstance()
	{
		if (_instance == nullptr)
		{
			_instance = new RedisManager();
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
	RedisManager();
	~RedisManager();

	static RedisManager *_instance;
	bool	isconnected;
};
