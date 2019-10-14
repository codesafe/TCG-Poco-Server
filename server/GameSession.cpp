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

	// �Ʒ��� Handler�� �ڵ� �غκп� ���� �Ѵ�.
	// �ڵ鷯�� ��ϵ��� ���� �۵��� �ϱ� ������ �ڵ鷯 ��� �Ʒ����� ������ �ϸ� �۵��� �̻������� ��� �ִ�.
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
	// ���⿡�� try/catch ������� ������ client socket reset�� ���� ������ ����
	// TimeoutException �� �߻��Ҽ��� �ִ�.
	// TODO. test : SOCKET_BUFFER�� �۰��ϰ� SOCKET_BUFFER���� ū ��Ŷ�� ������ ��� �Ǵ°� ---> ó���ؾ���
	// �̷���� ������ �̰��� ȣ��Ǵ°�?? �ƴϸ�?????

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

// Packet type�� ���� parser func�� func pointer �����ؾ� ��
bool GameSession::parsePacket()
{
/* 
	TODO.
	1. �޴����� ������ ���� �޾Ƽ� ó��
	2. ��Ŷ ������� ���� �޾ҳ�? �˻� --> �ƴϸ� ���޾ƾ� �Ѵ�.
	3. ����� ���뿡 ������ ����? --> �ƴϸ� disconnect
	3. ��Ŷ�� ũ�⸸ŭ �޾ҳ�? --> �ش� ��Ŷ �Ϸ�
	4. �������� �ִ°�??

*/

	while (true)
	{
		// ��� ���� �������� ������ ���޾ƾ� �ϱ⶧���� ��
		if (recvBuff->buffersize < sizeof(PacketHeader))
			return false;

		PacketHeader* header = (PacketHeader*)recvBuff->buffer;
		// ����� ��ȿ���� ���� ��� FALSE
		if (checkPacketHeader(header) == false)
			return false;

		int packetSize = header->packetsize;

		// ���� ���޾Ҵ� (��Ŷ�� ���̺��� ���� ���۰� �۴�)
		if (packetSize > recvBuff->buffersize)
			break;

 		// ��Ŷ�� ����� ����
		// ��Ŷ ó�� �ڵ鷯���� ����
		bool ret = PacketHandler::getInstance()->dispatchPacket(header->packetID, recvBuff->buffer, packetSize, guid);
		if (ret == false)
		{
			delete this;
			return false;
		}

		// ���� ��Ŷ���� ����
		recvBuff->flush(packetSize);
	}

	return true;
}

bool GameSession::checkPacketHeader(PacketHeader *header)
{
	/*
		�˻� �׸�
		Packet �����ڵ� (������/Ŭ���̾�Ʈ��)
		Packet ����
		Packet �Ϸù�ȣ (���������� �ʰų�, �ߺ�����)
		Packet ��ȣȭ ����

		�˻� �׸񿡼� �ϳ��� �߸��Ǹ� �ش� ������ ��ȿȭ(Close) �Ѵ�.
		��Ŷ�� �����Ͽ� Server�� �����ϰų� �Ҽ� �ֱ� ������ �� �κ��� �߿�
		* ��Ŷ�� ���̰� �ʹ� ��ų�, ���̰� �ȸ´°��, ���̴� ������ ������ ������ ������ �����͵� ����
	*/

	if (header == nullptr) return false;

	// Ŭ���̾�Ʈ���� �ƴ�?? �׷� ��
	if (header->signature != (char)CLIENT_SIG)
	{
		//Log::instance()->LogError("Wrong Header sig : %x", header->signature);
		return false;
	}

	// ��Ŷ �ø����� �ٸ��� ��
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

