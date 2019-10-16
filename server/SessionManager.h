#ifndef SESSION_MANAGER
#define SESSION_MANAGER

#include "Predef.h"
#include <vector>
#include <map>

//#define USE_STL

class GameSession;

/*
	POCO HashSet / HashMap은 STL보다 빠르다.
	https://pocoproject.org/slides/145-Hashing.pdf
*/

class _Session
{
public :
	enum SessionState
	{
		None = 0,
		Connected,
		Disconnected
	};

	_Session(USER_GUID guid, GameSession *handler);
	~_Session();

	SessionState getState();
	void setState(SessionState state);

	void sendProtoBuffer(const int packetID, const google::protobuf::Message& pb);
	void sendPacket(const int packetID, char *buf, int size);

private:
	SessionState	state;
	GameSession *handler;
	long guid;
};

//////////////////////////////////////////////////////////////////////////

class SessionManager
{
public :
	static SessionManager * getInstance()
	{
		if (_instance == nullptr)
		{
			_instance = new SessionManager();
		}
		return _instance;
	}


	USER_GUID addSession(GameSession *handler);
	bool removeSession(USER_GUID guid);
	// Needs readlock at outside usage.
	_Session * findSession(USER_GUID guid);

	bool broadCastAllSession(int packetID, char *buf, int size);
	void disconnnectSession(USER_GUID guid);

	void sendProtoBuffer(USER_GUID guid, int packetID, const google::protobuf::Message& pb);
	void sendBuffer(USER_GUID guid, int packetID, char *buf, int buflen);

	void lock(bool readlock);
	void unlock();

private:

#ifdef USE_STL
	std::map<USER_GUID, GameSession*>	sessionList;
#else
	Poco::HashMap<USER_GUID, _Session*>	sessionMap;
#endif

	USER_GUID userGuid;

	Poco::RWLock _lock;


private :
	SessionManager();
	~SessionManager();

	static SessionManager * _instance;
};


#endif