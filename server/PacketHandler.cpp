
#include "PacketHandler.h"
#include "Packet.h"
#include "SessionManager.h"
#include "GameSession.h"

PacketHandler *PacketHandler::_instance = nullptr;

PacketHandler::PacketHandler()
{
	initHandler();
}

PacketHandler::~PacketHandler()
{
}

// 이곳에서 패킷을 처리할 함수들을 등록
void PacketHandler::initHandler()
{
	this->addtReceiveCallBack(GameMsgID::MSG_REQ_CREATE_ACCOUNT, this, &PacketHandler::onCreateAccountReq);
	//this->addtReceiveCallBack(GameMsgID::MSG_REQ_LOGIN, this, &PacketHandler::onLoginReq);

	//WorldServer *worldserver = WorldServer::getInstance();
	//this->AddReceiveCallBack((uint32_t)FBPacket::NET_PACKET::NET_PACKET_MSG_REQ_MOVE_PLAYER, worldserver, &WorldServer::onMove);
}

template<typename FUNC_T>
bool PacketHandler::addtReceiveCallBack(const int packetID, FUNC_T* pBase, bool (FUNC_T::*handleRecieve)(const char* buf, const int buflen, const USER_GUID&))
{
	NET_RECEIVE_FUNCTOR functor = std::bind(handleRecieve, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	NET_RECEIVE_FUNCTOR_PTR functorPtr(new NET_RECEIVE_FUNCTOR(functor));
	return addtReceiveCallBack(packetID, functorPtr);
}

bool PacketHandler::addtReceiveCallBack(const int packetID, const NET_RECEIVE_FUNCTOR_PTR& cb)
{
	if (receiveCallBack.find(packetID) != receiveCallBack.end())
		return false;
	receiveCallBack.insert(std::map<int, NET_RECEIVE_FUNCTOR_PTR>::value_type(packetID, cb));
	return true;
}

// 패킷에 대한 함수 실행
bool PacketHandler::dispatchPacket(int packetid, char *buf, int buflen, USER_GUID guid)
{
	std::map<int, NET_RECEIVE_FUNCTOR_PTR>::iterator it = receiveCallBack.find(packetid);
	if (receiveCallBack.end() != it)
	{
		NET_RECEIVE_FUNCTOR_PTR& pFunPtr = it->second;
		(*pFunPtr)(buf, buflen, guid);
	}
	else
	{
		Application::instance().logger().information("--> Recv Packet GUID : %ld", guid);
		SessionManager::getInstance()->sendBuffer(guid, packetid, buf, buflen);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool PacketHandler::onCreateAccountReq(const char* buf, const int buflen, const USER_GUID& sessionID)
{
	return true;
}

bool PacketHandler::onLoginReq(const char* buf, const int buflen, const USER_GUID& sessionID)
{
	protobuf::io::ArrayInputStream is(buf, buflen);
	is.Skip(sizeof(PacketHeader));

	ReqLogin login;
	login.ParseFromZeroCopyStream(&is);

	std::string name = login.name();
	std::string pass = login.passwd();


	return true;
}

