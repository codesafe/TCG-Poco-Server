
#include "MySQLSessionPool.h"
#include "Common/Util.h"

MySQLSessionPool *MySQLSessionPool::_instance = nullptr;


MySQLSessionPool::MySQLSessionPool()
{
	mysqlpool = nullptr;
	maxSession = 0;
	idleTime = 0;

	Poco::Data::MySQL::Connector::registerConnector();
}

MySQLSessionPool::~MySQLSessionPool()
{

}

// "MySQL", "user=root;password=;db=calserver;compress=true;auto-reconnect=true", 1, 4, 5
bool	MySQLSessionPool::init(const std::string &connector, const std::string& connectionString, int minSessions, int maxSessions, int idleTime)
{
	this->maxSession = maxSessions;
	this->idleTime = idleTime;

	try
	{
		mysqlpool = new SessionPool("MySQL", connectionString, minSessions, maxSessions, idleTime);
	}
	catch (Poco::Exception &ex)
	{
		std::cerr << ex.displayText() << std::endl;
	}


	return true;
}

void	MySQLSessionPool::uninit()
{
	delete mysqlpool;
	Poco::Data::MySQL::Connector::unregisterConnector();
}

void MySQLSessionPool::dbInfo()
{
	Poco::Data::Session sess(mysqlpool->get());
	if (sess.isConnected())
	{
		UTIL::Log("=======================================================");
		UTIL::Log("Server Info : %s", Utility::serverInfo(sess));
		UTIL::Log("Server Version : %lu", Utility::serverVersion(sess));
		UTIL::Log("Host Info : %s", Utility::hostInfo(sess));
		UTIL::Log("=======================================================");
	}
}

SessionPool	*MySQLSessionPool::getSessionPool()
{
	int idle = mysqlpool->idle();
	int used = mysqlpool->used();
	int dead = mysqlpool->dead();
	int available = mysqlpool->available();

	UTIL::Log("Session Pool Status --> idle : %d , used : %d , available : %d", idle, used, available);

	return mysqlpool;
}

void	MySQLSessionPool::query(const std::string &query_str)
{
	try
	{
		Poco::Data::Session sess(mysqlpool->get());
		if (sess.isConnected())
		{
			std::string pass;
			sess << query_str.c_str(), into(pass), now;
			UTIL::Log("query test : %s", pass);
		}
		else
		{
			UTIL::Log("*** Connected to DB failed");
		}

	}
	catch (ConnectionException& ce)
	{
		UTIL::Log(ce.displayText());
	}
	catch (StatementException& se)
	{
		UTIL::Log(se.displayText());
	}
}

void	MySQLSessionPool::query(const std::string &query_str, Poco::Any &p1)
{
	try
	{
		Poco::Data::Session sess(mysqlpool->get());
		if (sess.isConnected())
		{
			std::string pass;
			sess << query_str.c_str(), into(pass), now;
		}
		else
		{
			UTIL::Log("*** Connected to DB failed");
		}
	}
	catch (ConnectionException& ce)
	{
		UTIL::Log(ce.displayText());
	}
	catch (StatementException& se)
	{
		UTIL::Log(se.displayText());
	}
}

void	MySQLSessionPool::query(const std::string &query_str, Poco::Any &p1, Poco::Any &p2)
{
	try
	{
		Poco::Data::Session sess(mysqlpool->get());
		if (sess.isConnected())
		{
			sess << query_str.c_str(), into(p1), into(p2), now;
		}
		else
		{
			UTIL::Log("*** Connected to DB failed");
		}
	}
	catch (ConnectionException& ce)
	{
		UTIL::Log(ce.displayText());
	}
	catch (StatementException& se)
	{
		UTIL::Log(se.displayText());
	}
}
