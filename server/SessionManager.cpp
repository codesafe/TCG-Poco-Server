#include "SessionManager.h"
#include "GameSession.h"
#include "Common/Util.h"


_Session::_Session(USER_GUID _guid, GameSession *_handler)
{
	guid = _guid;
	handler = _handler;
	state = SessionState::Connected;
}

_Session::~_Session()
{
	guid = 0xdeadbed;
}

_Session::SessionState _Session::getState()
{
	return state;
}

void _Session::setState(SessionState _state)
{
	state = _state;
}

void _Session::sendProtoBuffer(const int packetID, const google::protobuf::Message& pb)
{
	handler->sendProtoBuffer(packetID, pb);
}

void _Session::sendPacket(const int packetID, char *buf, int size)
{
	handler->sendBuffer(packetID, buf, size);
}


//////////////////////////////////////////////////////////////////////////

SessionManager * SessionManager::_instance = nullptr;

SessionManager::SessionManager()
{
	userGuid = 1000;
}
			   
SessionManager::~SessionManager()
{
}

USER_GUID SessionManager::addSession(GameSession *handler)
{
	_lock.writeLock();
	USER_GUID retGuid = userGuid;

#ifdef USE_STL
	sessionList.insert(std::make_pair(userGuid, handler));
#else
	_Session *sess = new _Session(userGuid, handler);
	sessionMap[userGuid] = sess;
#endif

	userGuid++;
	_lock.unlock();

	return retGuid;
}

bool SessionManager::removeSession(USER_GUID guid)
{
	bool ret = true;
	_lock.writeLock();

#ifdef USE_STL
	std::map<USER_GUID, GameSession*>::iterator it = sessionList.find(guid);
	if (it != sessionList.end())
	{
		sessionList.erase(it);
		UTIL::Log("removeSession : %d", (int)sessionList.size());
	}
	else
	{
		UTIL::Log("Not found : %ld", guid);
		ret = false; // not found user
	}
#else
	Poco::HashMap<USER_GUID, _Session*>::Iterator itr = sessionMap.find(guid);
	if (sessionMap.end() != itr)
	{
		sessionMap.erase(itr);
		UTIL::Log("removeSession : %d", (int)sessionMap.size());
	}
	else
	{
		UTIL::Log("Not found : %ld", guid);
		ret = false; // not found user
	}
#endif

	_lock.unlock();

	return ret;
}

// 이 함수 사용하는 곳 외부에서 readlock 이 필요하다.
_Session * SessionManager::findSession(USER_GUID guid)
{
	_lock.readLock();
#ifdef USE_STL
	GameSession *user = nullptr;
	std::map<USER_GUID, GameSession*>::iterator it = sessionList.find(guid);
	if (it != sessionList.end())
	{
		user = it->second;
	}
#else
	_Session *user = nullptr;
	Poco::HashMap<USER_GUID, _Session*>::Iterator itr = sessionMap.find(guid);
	if (sessionMap.end() != itr)
	{
		user = itr->second;
	}
#endif
	_lock.unlock();

	return user;
}

bool SessionManager::broadCastAllSession(int packetID, char *buf, int size)
{
	_lock.readLock();
	Poco::HashMap<USER_GUID, _Session*>::Iterator itr = sessionMap.begin();
	for(; itr != sessionMap.end(); itr++)
	{
		if( itr->second->getState() == _Session::Connected)
			itr->second->sendPacket(packetID, buf, size);
	}
	_lock.unlock();

	return true;
}

void SessionManager::disconnnectSession(USER_GUID guid)
{
	_lock.writeLock();
#ifdef USE_STL
	std::map<USER_GUID, GameSession*>::iterator it = sessionList.find(guid);
	if (it != sessionList.end())
	{
		//it->second->setState(Session::SessionState::Disconnected);
	}
#else
	Poco::HashMap<USER_GUID, _Session*>::Iterator itr = sessionMap.find(guid);
	if (sessionMap.end() != itr)
	{
		itr->second->setState(_Session::SessionState::Disconnected);
	}
#endif
	_lock.unlock();
}

void SessionManager::sendProtoBuffer(USER_GUID guid, int packetID, const google::protobuf::Message& pb)
{
	_lock.readLock();
#ifdef USE_STL
	std::map<USER_GUID, GameSession*>::iterator it = sessionList.find(guid);
	if (it != sessionList.end())
	{
		GameSession *user = it->second;
		user->sendProtoBuffer(packetID, pb);
	}
#else
	Session *user = nullptr;
	Poco::HashMap<USER_GUID, _Session*>::Iterator itr = sessionMap.find(guid);
	if (sessionMap.end() != itr)
	{
		itr->second->sendProtoBuffer(packetID, pb);
	}
#endif
	_lock.unlock();
}

void SessionManager::sendBuffer(USER_GUID guid, int packetID, char *buf, int buflen)
{
	_lock.readLock();
#ifdef USE_STL
	std::map<USER_GUID, GameSession*>::iterator it = sessionList.find(guid);
	if (it != sessionList.end())
	{
		GameSession *user = it->second;
		user->sendBuffer(packetID, buf, buflen);
	}
#else
	Session *user = nullptr;
	Poco::HashMap<USER_GUID, _Session*>::Iterator itr = sessionMap.find(guid);
	if (sessionMap.end() != itr)
	{
		itr->second->sendPacket(packetID, buf, buflen);
	}
#endif
	_lock.unlock();
}

void SessionManager::lock(bool readlock)
{
	if(readlock)
		_lock.readLock();
	else
		_lock.writeLock();
}

void SessionManager::unlock()
{
	_lock.unlock();
}
