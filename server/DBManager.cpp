#if 1

#include "DBManager.h"

using Poco::Redis::Client;
using Poco::Redis::Array;
using Poco::Redis::Command;

DBManager *DBManager::_instance = nullptr;

DBManager::DBManager() : isconnected(false)
{

}

DBManager::~DBManager()
{
	client.disconnect();
}

bool DBManager::init(const std::string& host, int port)
{
	if (isconnected) return true;

	Timespan timeout(5000000);	// 5sec
	client.connect(host, port, timeout);
	isconnected = true;

	return true;
}

bool DBManager::authRedis(const std::string& pw)
{
	Array cmd;
	cmd << "AUTH" << pw;
	std::string response;

	try 
	{
		response = client.execute<std::string>(cmd);
	}
	catch (...) 
	{
		return  false;
	}

	return (response == "OK");
}

bool DBManager::sendCommand(const std::string& command)
{
	//HSETNX GUID 1 12

	Application& app = Application::instance();

	Array cmd;
	cmd << "SET" << "name" << "anan";
	std::string response;
	try
	{
		response = client.execute<std::string>(cmd);
		app.logger().information("Redis Command : %s", response);
	}
	catch (Poco::BadCastException &e)
	{
		app.logger().information("Redis Command Error : %s", e.displayText());
	}
	catch (Poco::Redis::RedisException &e)
	{
		app.logger().information("Redis Command Error : %s", e.displayText());
	}
	catch (...)
	{
		return false;
	}

	return true;
}

Poco::Redis::BulkString DBManager::getFromHashTBL(const std::string& tbl, const std::string& key)
{
	Application& app = Application::instance();

	Command cmd = Command::hget(tbl, key);
	Poco::Redis::BulkString response;

	try
	{
		response = client.execute<Poco::Redis::BulkString>(cmd);
		if(!response.isNull())
			app.logger().information("Redis Command : %s", response);
	}
	catch (Poco::BadCastException &e)
	{
		// 리턴하는 값의 타입이 잘못되었다. 다른 형을사용해야 한다.
		// supported types are Int64, std::string, BulkString, Array and void.
		app.logger().information("Redis Command Error : %s", e.displayText());
	}
	catch (Poco::Redis::RedisException &e)
	{
		app.logger().information("Redis Command Error : %s", e.displayText());
	}
	catch (...)
	{
		return response;
	}
	return response;
}

#endif