#pragma  once

#include "Predef.h"

struct SocketBuffer;

class LoginServer
{
public:
	static LoginServer * getInstance()
	{
		if (_instance == nullptr)
		{
			_instance = new LoginServer();
		}
		return _instance;
	}


public :

	void onLogin(const SocketBuffer* socketbuffer, const USER_GUID& clientID);

private :
	LoginServer();
	~LoginServer();

	static LoginServer * _instance;
};