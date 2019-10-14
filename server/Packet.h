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
	char	signature;		// 서버가 보낸것?(SERVER_SIG) 클라이언트가 보낸것? (CLIENT_SIG)
	int		packetsize;		// packet size는 header + data 포함한 전체 길이
	int		packetserial;	// 생성 번호 = wCommandID^dwSize+index(패키지당 자동 성장 색인); 환원 번호 = pHeader->dwPacketNo - pHeader->wCommandID^pHeader->dwSize;
	int		packetID;			// msg ID
};

#pragma  pack(pop)




#endif
