#pragma once

#include "Predef.h"

struct SocketBuffer;

class WorldServer
{
public:
	static WorldServer * getInstance()
	{
		if (_instance == nullptr)
		{
			_instance = new WorldServer();
		}
		return _instance;
	}

	void onMove(const SocketBuffer* socketbuffer, const USER_GUID& clientID);

private:

	Poco::RWLock _lock;


private:
	WorldServer();
	~WorldServer();

	static WorldServer * _instance;
};

