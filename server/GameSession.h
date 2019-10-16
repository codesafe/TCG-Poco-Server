#ifndef GAME_SESSION
#define GAME_SESSION


#include "Predef.h"
#include <vector>
#include "SocketBuffer.h"
#include "Packet.h"

class DataBuff;

class GameSession
{
public:
	GameSession(StreamSocket& socket, SocketReactor& reactor);
	~GameSession();

	void onShutdown(ShutdownNotification* pNotification);
	void onReadable(ReadableNotification* pNotification);
	void onWritable(WritableNotification* pNotification);
	void onError(ErrorNotification* pNotification);
	void onTimeout(TimeoutNotification* pNotification);
	void onIdle(IdleNotification* pNotification);

	void sendProtoBuffer(int packetid, const google::protobuf::Message& pb);
	void sendBuffer(int packetid, char *buf, int size);

private:
	bool	parsePacket();
	bool	checkPacketHeader(PacketHeader *header);

private:

	StreamSocket   _socket;
	SocketReactor& _reactor;

	//char      receiveBuf[BUFFER_SIZE];
	//int		_receivesize;

	DataBuff*	recvBuff;

	char        sendBuf[SOCKET_BUFFER];
	int			_sendsize;

	bool		writableAdded;
	bool		timeoutAdded;

	USER_GUID		guid;
	unsigned long	packetSerial;
};

#endif