
#include "Predef.h"
#include "GameSession.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"Iphlpapi.lib")

#if 1

class Server : public Poco::Util::ServerApplication
{
public:
	Server(){}
	~Server(){}

protected:

	int main(const std::vector<std::string>& args)
	{
		Application::instance().logger().information("Start Server!!");
		int threadnum = 2;
		unsigned short port = 9000;
		ServerSocket svs(port);

#if 1
		Timespan timeout(10, 0);
		SocketReactor reactor(timeout);
		ParallelSocketAcceptor<GameSession, SocketReactor> acceptor(svs, reactor, threadnum);
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

		return Application::EXIT_OK;
	}
};


int main(int argc, char** argv)
{
	Server app;
	return app.run(argc, argv);
}

#else

const Poco::UInt16 PORT = 9000;

class _Session
{
public:
	_Session(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor) :
		m_Socket(socket), m_Reactor(reactor)
	{
		m_PeerAddress = socket.peerAddress().toString();
		std::cout << "connection from " << m_PeerAddress << " ..." << std::endl;

		// 데이터 읽기 가능 상태가 된 호출할 핸들러를 등록한다.
		m_Reactor.addEventHandler(m_Socket,
			Poco::Observer<_Session, Poco::Net::ReadableNotification>(*this, &_Session::onReadable));

		// SocketReactor가 셧다운 할 때 발생하는 이벤트
		m_Reactor.addEventHandler(m_Socket,
			Poco::Observer<_Session, Poco::Net::ShutdownNotification>(*this, &_Session::onShutdown));

		// 소켓에 오류가 생겼을 때 발생하는  이벤트
		m_Reactor.addEventHandler(m_Socket,
			Poco::Observer<_Session, Poco::Net::ErrorNotification>(*this, &_Session::onError));

		// Poco::Net::select 호출 시 react 할 소켓이 없는 경우 발생하는 이벤트
		m_Reactor.addEventHandler(m_Socket,
			Poco::Observer<_Session, Poco::Net::IdleNotification>(*this, &_Session::onIdle));

		// Poco::Net::select 호출 후 지정 시간까지 이벤트가 없을 때 발생하는 이벤트
		m_Reactor.addEventHandler(m_Socket,
			Poco::Observer<_Session, Poco::Net::TimeoutNotification>(*this, &_Session::onTimeout));
	}

	~_Session()
	{
		std::cout << m_PeerAddress << " disconnected ..." << std::endl;

		// 생성자에서 등록한 핸들러를 해제해준다.
		m_Reactor.removeEventHandler(m_Socket, Poco::Observer<_Session, Poco::Net::ReadableNotification>(*this, &_Session::onReadable));

		m_Reactor.removeEventHandler(m_Socket, Poco::Observer<_Session, Poco::Net::ShutdownNotification>(*this, &_Session::onShutdown));

		m_Reactor.removeEventHandler(m_Socket, Poco::Observer<_Session, Poco::Net::ErrorNotification>(*this, &_Session::onError));

		m_Reactor.removeEventHandler(m_Socket, Poco::Observer<_Session, Poco::Net::IdleNotification>(*this, &_Session::onIdle));

		m_Reactor.removeEventHandler(m_Socket, Poco::Observer<_Session, Poco::Net::TimeoutNotification>(*this, &_Session::onTimeout));
	}

	// 읽기 가능 핸들러에 대한 정의.
	void onReadable(Poco::Net::ReadableNotification* pNotification)
	{
		// 해당 이벤트를 처리했음을 Reactor프레임워크에 알린다.
		pNotification->release();

		try
		{
			char buffer[256] = { 0 };

			int n = m_Socket.receiveBytes(buffer, sizeof(buffer));

			if (n > 0)
			{
				char szSendMessage[256] = { 0 };
				sprintf_s(szSendMessage, 256 - 1, "Re:%s", buffer);

				int nMsgLen = (int)strnlen_s(szSendMessage, 256 - 1);

				m_Socket.sendBytes(szSendMessage, nMsgLen);

				std::cout << "클라이언트에서 받은 메시지: " << buffer << std::endl;
			}
			else
			{
				m_Socket.shutdown();
				delete this;                    // 메모리를 해제하지 않으면 소멸자가 호출이 안된다.
			}
		}
		catch (Poco::Exception& exc)
		{
			m_Socket.shutdown();
			delete this;
		}
	}

	void onShutdown(Poco::Net::ShutdownNotification* pNf)
	{
		pNf->release();

		std::cout << "onShutdown 발생" << std::endl;
	}

	void onError(Poco::Net::ErrorNotification* pNf)
	{
		pNf->release();

		std::cout << "onError 발생" << std::endl;
	}

	void onTimeout(Poco::Net::TimeoutNotification* pNf)
	{
		pNf->release();

		std::cout << "onTimeout 발생" << std::endl;
	}

	void onIdle(Poco::Net::IdleNotification* pNf)
	{
		pNf->release();

		std::cout << "onIdle 발생" << std::endl;
	}

private:
	Poco::Net::StreamSocket m_Socket;
	std::string m_PeerAddress;
	Poco::Net::SocketReactor& m_Reactor;
};


void main()
{
	Poco::Timespan timeout(1, 0);

	Poco::Net::SocketReactor reactor(timeout);
	Poco::Net::ServerSocket serverSocket(PORT);

	// 서버는 클라이언트를 reactor 접속 방식으로 처리한다.
	Poco::Net::ParallelSocketAcceptor<_Session, Poco::Net::SocketReactor> acceptor(serverSocket, reactor);

	std::cout << "Reactor: starting... " << std::endl;

	// 서버는 대기 상태로 빠진다.
	// 대기 상태를 빠져나오기 위해서는 reactor.stop()을 호출하면 된다.
	reactor.run();

	// 서버가 종료되기 전까지 더 이상 아래 코드는 진행되지 않는다.
	std::cout << "Reactor: 종료..." << std::endl;
	getchar();
}


#endif