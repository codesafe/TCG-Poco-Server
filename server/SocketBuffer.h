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

	int		totalsize;      // ��ü ����
	int		currentsize;    // �������� ���� ��ġ (��/���� ���)
	uint32_t packetid;

	char buffer[SOCKET_BUFFER];
};

