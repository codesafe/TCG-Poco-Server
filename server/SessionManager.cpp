#include "SessionManager.h"
#include "GameSession.h"

Session::Session(long _guid, GameSession *_handler)
{
	guid = _guid;
	handler = _handler;
	state = SessionState::Connected;
}

Session::~Session()
{
	guid = 0xdeadbed;
}

Session::SessionState Session::getState()
{
	return state;
}

void Session::setState(SessionState _state)
{
	state = _state;
}

void Session::sendPacket(const int packetID, char *buf, int size)
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

long SessionManager::addSession(GameSession *handler)
{
	_lock.writeLock();
	USER_GUID retGuid = userGuid;

#ifdef USE_STL
	sessionList.insert(std::make_pair(userGuid, handler));
#else
	Session *sess = new Session(userGuid, handler);
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
	std::map<long, GameSession*>::iterator it = sessionList.find(guid);
	if (it != sessionList.end())
	{
		sessionList.erase(it);
		Application::instance().logger().information("removeSession : %d", (int)sessionList.size());
	}
	else
	{
		Application::instance().logger().information("Not found : %ld", guid);
		ret = false; // not found user
	}
#else
	Poco::HashMap<USER_GUID, Session*>::Iterator itr = sessionMap.find(guid);
	if (sessionMap.end() != itr)
	{
		sessionMap.erase(itr);
		Application::instance().logger().information("removeSession : %d", (int)sessionMap.size());
	}
	else
	{
		Application::instance().logger().information("Not found : %ld", guid);
		ret = false; // not found user
	}
#endif

	_lock.unlock();

	return ret;
}

// 이 함수 사용하는 곳 외부에서 readlock 이 필요하다.
Session * SessionManager::findSession(USER_GUID guid)
{
	_lock.readLock();
#ifdef USE_STL
	GameSession *user = nullptr;
	std::map<long, GameSession*>::iterator it = sessionList.find(guid);
	if (it != sessionList.end())
	{
		user = it->second;
	}
#else
	Session *user = nullptr;
	Poco::HashMap<USER_GUID, Session*>::Iterator itr = sessionMap.find(guid);
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
	Poco::HashMap<USER_GUID, Session*>::Iterator itr = sessionMap.begin();
	for(; itr != sessionMap.end(); itr++)
	{
		if( itr->second->getState() == Session::Connected)
			itr->second->sendPacket(packetID, buf, size);
	}
	_lock.unlock();

	return true;
}

void SessionManager::disconnnectSession(USER_GUID guid)
{
	_lock.writeLock();
#ifdef USE_STL
	std::map<long, GameSession*>::iterator it = sessionList.find(guid);
	if (it != sessionList.end())
	{
		//it->second->setState(Session::SessionState::Disconnected);
	}
#else
	Poco::HashMap<USER_GUID, Session*>::Iterator itr = sessionMap.find(guid);
	if (sessionMap.end() != itr)
	{
		itr->second->setState(Session::SessionState::Disconnected);
	}
#endif
	_lock.unlock();
}

void SessionManager::sendBuffer(USER_GUID guid, int packetID, char *buf, int buflen)
{
	_lock.readLock();
#ifdef USE_STL
	std::map<long, GameSession*>::iterator it = sessionList.find(guid);
	if (it != sessionList.end())
	{
		GameSession *user = it->second;
		user->sendBuffer(packetID, buf, buflen);
	}
#else
	Session *user = nullptr;
	Poco::HashMap<USER_GUID, Session*>::Iterator itr = sessionMap.find(guid);
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
