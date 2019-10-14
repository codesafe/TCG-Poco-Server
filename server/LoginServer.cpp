#include "LoginServer.h"
#include "SocketBuffer.h"

LoginServer * LoginServer::_instance = nullptr;

LoginServer::LoginServer()
{

}

LoginServer::~LoginServer()
{

}

void LoginServer::onLogin(const SocketBuffer* socketbuffer, const USER_GUID& clientID)
{

}