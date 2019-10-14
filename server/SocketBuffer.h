#pragma once

#include "Predef.h"

struct SocketBuffer
{
	SocketBuffer() 
	{
		totalsize = 0;
		currentsize = 0;
		packetid = 0;
	}

	int		totalsize;      // 전체 길이
	int		currentsize;    // 진행중인 버퍼 위치 (송/수신 모두)
	uint32_t packetid;

	char buffer[SOCKET_BUFFER];
};

