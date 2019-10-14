#ifndef PACKET_HANDLER
#define PACKET_HANDLER

#include "Predef.h"

class PacketHandler
{
public:
	static PacketHandler * getInstance()
	{
		if (_instance == nullptr)
			_instance = new PacketHandler();
		return _instance;
	}

	bool dispatchPacket(int packetid, char *buf, int buflen, USER_GUID guid);
	bool onCreateAccountReq(const char* buf, const int buflen, const USER_GUID& sessionID);
	bool onLoginReq(const char* buf, const int buflen, const USER_GUID& sessionID);


private:
	PacketHandler();
	~PacketHandler();

	static PacketHandler * _instance;
	void initHandler();

private:
	template<typename FUNC_T>
	bool addtReceiveCallBack(const int packetID, FUNC_T* pBase, bool (FUNC_T::*handleRecieve)(const char* buf, const int buflen, const USER_GUID&));
	bool addtReceiveCallBack(const int packetID, const NET_RECEIVE_FUNCTOR_PTR& cb);
	std::map<int, NET_RECEIVE_FUNCTOR_PTR> receiveCallBack;
};



#endif
