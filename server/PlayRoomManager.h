#ifndef PLAY_ROOM_MANAGER
#define PLAY_ROOM_MANAGER


#include "Predef.h"

enum PLAYROOM_STATE
{
	STATE_NONE = 0,
	STATE_PLAYING = 1,
	STATE_END = 2
};

class PlayRoom
{
public:
	PlayRoom();
	~PlayRoom();

private:
	PLAYROOM_ID		roomID;
	USER_GUID		users[2];

	PLAYROOM_STATE	state;
};


//////////////////////////////////////////////////////////////////////////

class PlayRoomManager
{
public:
	static PlayRoomManager * getInstance()
	{
		if (_instance == nullptr)
		{
			_instance = new PlayRoomManager();
		}
		return _instance;
	}

	void	init(int num);



private:
	Poco::RWLock _lock;

private:
	PlayRoomManager();
	~PlayRoomManager();

	static PlayRoomManager * _instance;

#ifdef USE_STL
	std::map<PLAYROOM_ID, PlayRoom*>	sessionList;
#else
	Poco::HashMap<PLAYROOM_ID, PlayRoom*>	playRoomList;
#endif

};


#endif