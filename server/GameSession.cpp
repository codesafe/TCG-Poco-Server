#include "GameSession.h"
#include "SessionManager.h"
#include "LoginServer.h"
#include "WorldServer.h"
#include "DBManager.h"
#include "PacketHandler.h"

#include "DataBufferPool.h"
#include "Packet.h"

GameSession::GameSession(StreamSocket& socket, SocketReactor& reactor)
	:_socket(socket)
	, _reactor(reactor)
	, writableAdded(false)
	, timeoutAdded(false)
{
	Application& app = Application::instance();
	//app.logger().information("Connection from " + socket.peerAddress().toString());

	guid = SessionManager::getInstance()->addSession(this);
	app.logger().information("Connection GUID : %ld", guid);

	recvBuff = DataBufferManager::getInstance()->allocBuffer(0);

	Timespan timeout(10, 0);
	_reactor.setTimeout(timeout);

	_socket.setNoDelay(false);
	_socket.setReusePort(true);
	_socket.setReuseAddress(true);

	// 아래의 Handler는 코드 밑부분에 들어가야 한다.
	// 핸들러가 등록되자 마자 작동을 하기 때문에 핸들러 등록 아래에서 뭔가를 하면 작동이 이상해지는 경우 있다.
	_reactor.addEventHandler(_socket, Observer<GameSession, ReadableNotification>(*this, &GameSession::onReadable));
	_reactor.addEventHandler(_socket, Observer<GameSession, ShutdownNotification>(*this, &GameSession::onShutdown));
	_reactor.addEventHandler(_socket, Observer<GameSession, ErrorNotification>(*this, &GameSession::onError));
	_reactor.addEventHandler(_socket, Observer<GameSession, IdleNotification>(*this, &GameSession::onIdle));
	if(timeoutAdded)
		_reactor.addEventHandler(_socket, Observer<GameSession, TimeoutNotification>(*this, &GameSession::onTimeout));

/*
	DBManager::getInstance()->init("192.168.29.183", 6379);
	//DBManager::getInstance()->sendCommand("");
	Poco::Redis::BulkString ret = DBManager::getInstance()->getFromHashTBL("GUID","32");

	if(!ret.isNull())
		app.logger().information("FROM DB : %s", ret);
*/
}

GameSession::~GameSession()
{
	Application& app = Application::instance();

	//app.logger().information("Disconnecting " + _socket.peerAddress().toString());
	app.logger().information("--> Disconnecting GUID : %ld", guid);

	_reactor.removeEventHandler(_socket, Observer<GameSession, ReadableNotification>(*this, &GameSession::onReadable));
	_reactor.removeEventHandler(_socket, Observer<GameSession, ShutdownNotification>(*this, &GameSession::onShutdown));
	_reactor.removeEventHandler(_socket, Observer<GameSession, ErrorNotification>(*this, &GameSession::onError));
	_reactor.removeEventHandler(_socket, Observer<GameSession, IdleNotification>(*this, &GameSession::onIdle));

	if(timeoutAdded)
		_reactor.removeEventHandler(_socket, Observer<GameSession, TimeoutNotification>(*this, &GameSession::onTimeout));

	if (writableAdded)
		_reactor.removeEventHandler(_socket, Observer<GameSession, WritableNotification>(*this, &GameSession::onWritable));

	SessionManager::getInstance()->disconnnectSession(guid);
	SessionManager::getInstance()->removeSession(guid);

	DataBufferManager::getInstance()->releaseBuffer(recvBuff);
	recvBuff = nullptr;
}

void GameSession::onReadable(Poco::Net::ReadableNotification* pNotification)
{
	// 여기에서 try/catch 사용하지 않으면 client socket reset시 무한 루프에 빠짐
	// TimeoutException 이 발생할수도 있다.
	// TODO. test : SOCKET_BUFFER를 작게하고 SOCKET_BUFFER보다 큰 패킷이 왔을때 어떻게 되는가 ---> 처리해야함
	// 이럴경우 여러번 이곳이 호출되는가?? 아니면?????

	pNotification->release();

	try
	{
		char tempbuf[SOCKET_BUFFER];
		int recvsize = _socket.receiveBytes(tempbuf, SOCKET_BUFFER);
		if (recvsize > 0)
		{
			recvBuff->copyData(tempbuf, recvsize);
			parsePacket();
		}
		else
		{
			// graceful shutdown
			_socket.shutdown();
			delete this;
		}

	}
	//catch (...)
	catch (Poco::Exception& exc)
	{
		_socket.shutdown();
		delete this;
	}
}

void GameSession::onShutdown(Poco::Net::ShutdownNotification* pNotification)
{
	pNotification->release();
	delete this;
}


void GameSession::onWritable(Poco::Net::WritableNotification* pNotification)
{
	//Application::instance().logger().information("onWritable called");
//	std::string data = Poco::format("received %s", _data);
//	_socket.sendBytes(data.data(), data.length());

	pNotification->release();

	try
	{
		int totalsent = 0;
		while (1)
		{
			int sent = _socket.sendBytes(sendBuf+totalsent, _sendsize);
			if (sent < 0)
			{
				// Disconnected
				//delete this;
				_socket.shutdown();
				break;
			}
			else
			{
				totalsent += sent;
				if (totalsent < _sendsize)
					_sendsize -= sent;
				else
					break;	// done
			}
		}

		//_reactor.removeEventHandler(_socket, Poco::Observer<GameSession, WritableNotification>(*this, &GameSession::onWritable));

// 		_reactor.removeEventHandler(_socket, NObserver<GameSession, WritableNotification>(*this, &GameSession::onWritable));
// 		if (_onTimeoutAdded) {
// 			_reactor.removeEventHandler(_socket, NObserver<GameSession, TimeoutNotification>(*this, &GameSession::onTimeout));
// 		}

// 		if (sent < 0)
// 			Application::instance().logger().information("onWritable Sent :  %d", sent);

		/*
		_reactor.addEventHandler(_socket, NObserver<GameserverHandler, TimeoutNotification>(*this, &GameserverHandler::onTimeout));
		_onTimeoutAdded = true;
		*/
	}
	catch (...)
	{
		_socket.shutdown();
		// DisConnected
		delete this;
	}
}

void GameSession::onError(Poco::Net::ErrorNotification* pNotification)
{
	pNotification->release();
	Application::instance().logger().information("onError called");
}

void GameSession::onTimeout(Poco::Net::TimeoutNotification* pNotification)
{
	pNotification->release();
	Application::instance().logger().information("onTimeout called");

	// check alive
// 	_sendsize = 0;
// 	_reactor.removeEventHandler(_socket, Poco::Observer<GameSession, WritableNotification>(*this, &GameSession::onWritable));
//	writableAdded = true;
}

void GameSession::onIdle(Poco::Net::IdleNotification* pNotification)
{
	pNotification->release();
	Application::instance().logger().information("onIdle called");

}

void GameSession::sendBuffer(int packetid, char *buf, int size)
{
	int packetlen = size + sizeof(int) + sizeof(uint32_t);

	memcpy(sendBuf, &packetlen, sizeof(int));
	memcpy(sendBuf + sizeof(int), &packetid, sizeof(uint32_t));
	memcpy(sendBuf+sizeof(int)+sizeof(uint32_t), buf, size);

	_sendsize = packetlen;

	_reactor.removeEventHandler(_socket, Poco::Observer<GameSession, WritableNotification>(*this, &GameSession::onWritable));
	writableAdded = true;
}

// Packet type에 따른 parser func를 func pointer 연결해야 함
bool GameSession::parsePacket()
{
/* 
	TODO.
	1. 받던것이 없으면 새로 받아서 처리
	2. 패킷 헤더보다 많이 받았나? 검사 --> 아니면 더받아야 한다.
	3. 헤더의 내용에 문제가 없나? --> 아니면 disconnect
	3. 패킷의 크기만큼 받았나? --> 해당 패킷 완료
	4. 남은것이 있는가??

*/

	while (true)
	{
		// 헤더 보다 받은것이 작으면 더받아야 하기때문에 끝
		if (recvBuff->buffersize < sizeof(PacketHeader))
			return false;

		PacketHeader* header = (PacketHeader*)recvBuff->buffer;
		// 헤더가 유효하지 않은 경우 FALSE
		if (checkPacketHeader(header) == false)
			return false;

		int packetSize = header->packetsize;

		// 아직 덜받았다 (패킷의 길이보다 받은 버퍼가 작다)
		if (packetSize > recvBuff->buffersize)
			break;

 		// 패킷을 충분히 받음
		// 패킷 처리 핸들러에게 전달
		bool ret = PacketHandler::getInstance()->dispatchPacket(header->packetID, recvBuff->buffer, packetSize, guid);
		if (ret == false)
		{
			delete this;
			return false;
		}

		// 사용된 패킷버퍼 정리
		recvBuff->flush(packetSize);
	}

	return true;
}

bool GameSession::checkPacketHeader(PacketHeader *header)
{
	/*
		검사 항목
		Packet 고유코드 (서버용/클라이언트용)
		Packet 길이
		Packet 일련번호 (연속적이지 않거나, 중복여부)
		Packet 암호화 여부

		검사 항목에서 하나라도 잘못되면 해당 연결을 무효화(Close) 한다.
		패킷을 변조하여 Server를 공격하거나 할수 있기 때문에 이 부분은 중요
		* 패킷의 길이가 너무 길거나, 길이가 안맞는경우, 길이는 같으나 나머지 내용이 쓰레기 데이터도 포함
	*/

	if (header == nullptr) return false;

	// 클라이언트용이 아님?? 그럼 끝
	if (header->signature != (char)CLIENT_SIG)
	{
		//Log::instance()->LogError("Wrong Header sig : %x", header->signature);
		return false;
	}

	// 패킷 시리얼이 다르면 끝
// 	if( header->packetserial > packetSerial)
// 	{
// 		Log::instance()->LogError("Wrong Packet serial : %d : %d", packetSerial, header->packetserial);
// 		return FALSE;
// 	}

	if (header->packetsize > SOCKET_BUFFER)
	{
		//Log::instance()->LogError("Over Packet size : %d", header->packetsize);
		return false;
	}

	return true;
}

