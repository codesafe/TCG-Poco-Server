#ifndef PACKET
#define PACKET

#include "Predef.h"

// #define PROCESS_MESSAGE(msgID, Func) \
// 		case msgID:{\
// 		if(Func(buf, buflen, guid)){return TRUE;}} break;

#define CLIENT_SIG 0xC0
#define SERVER_SIG 0xDE

#pragma  pack(push)
#pragma  pack(1)


struct PacketHeader
{
	char	signature;		// ������ ������?(SERVER_SIG) Ŭ���̾�Ʈ�� ������? (CLIENT_SIG)
	int		packetsize;		// packet size�� header + data ������ ��ü ����
	int		packetserial;	// ���� ��ȣ = wCommandID^dwSize+index(��Ű���� �ڵ� ���� ����); ȯ�� ��ȣ = pHeader->dwPacketNo - pHeader->wCommandID^pHeader->dwSize;
	int		packetID;			// msg ID
};

#pragma  pack(pop)




#endif
