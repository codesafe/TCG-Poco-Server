
#include "Predef.h"
#include "GameSession.h"
#include "Common/Util.h"
#include "MySQLSessionPool.h"


class Server : public Poco::Util::ServerApplication
{
public:
	Server(){}
	~Server(){}

protected:
	bool initDB()
	{
		// Connection string
		std::string dbConnString = 
			"host=" MYSQL_HOST
			";user=" MYSQL_USER
			";password=" MYSQL_PWD
			";db=" MYSQL_DB
			";compress=true"
			";auto-reconnect=true"
			";secure-auth=true"
			";protocol=tcp";

		return MySQLSessionPool::getInstance()->init("MySQL", dbConnString, 1, 30, 10);
	}

	int main(const std::vector<std::string>& args)
	{
		if (initDB())
		{
			UTIL::Log("DataBase Start Server!!");
			MySQLSessionPool::getInstance()->dbInfo();
			//MysqlSessionPool::getInstance()->query("SELECT pass FROM LOGIN WHERE id='codesafe'");
		}

		UTIL::Log("========  Start Server!!  ========");
		ServerSocket svs(SERVER_PORT);
#if 1
		Timespan timeout(10, 0);
		SocketReactor reactor(timeout);
		ParallelSocketAcceptor<GameSession, SocketReactor> acceptor(svs, reactor, ACCEPT_THREAD_NUM);
		reactor.run();
#else

		//Timespan timeout(10000000); // timeout is set to 10sec
		Timespan timeout(10, 0);

		SocketReactor reactor(timeout);

		//SocketAcceptor<GameserverHandler> acceptor(svs, reactor);
		ParallelSocketAcceptor<GameSession, SocketReactor> acceptor(svs, reactor, threadnum);

		Thread thread;
		thread.start(reactor);
		waitForTerminationRequest();
		reactor.stop();
		thread.join();
#endif

		MySQLSessionPool::getInstance()->uninit();
		return Application::EXIT_OK;
	}
};


int main(int argc, char** argv)
{
	Server app;
	return app.run(argc, argv);
}

