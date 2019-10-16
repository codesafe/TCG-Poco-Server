﻿
#include "PacketHandler.h"
#include "Packet.h"
#include "SessionManager.h"
#include "GameSession.h"
#include "Common/Util.h"
#include "MySQLSessionPool.h"


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
	this->addtReceiveCallBack(GameMsgID::MSG_REQ_LOGIN, this, &PacketHandler::onLoginReq);

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
		UTIL::Log("--> Recv Packet GUID : %ld", guid);
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

	//////////////////////////////////////////////////////////////////////////

	std::string querystr = "select * from account where name = ?";

	USER_GUID ret_gsn;
	std::string ret_name;
	std::string ret_pass;

	try
	{
		Poco::Data::Session sess(MySQLSessionPool::getInstance()->getSessionPool()->get());
		if (sess.isConnected())
		{
			//Packet_LOGIN login;
			//sess << querystr, into(login), use(name), now;
			sess << querystr, into(ret_gsn), into(ret_name), into(ret_pass), use(name), now;
		}
		else
			UTIL::Log("*** Connected to DB failed");
	}
	catch (ConnectionException& ce)
	{
		UTIL::Log(ce.displayText());
	}
	catch (StatementException& se)
	{
		UTIL::Log(se.displayText());
	}

	//////////////////////////////////////////////////////////////////////////

	AckLogin ack;
	if( pass == ret_pass )
		ack.set_result(RET_LOGIN_SUCCESS);
	else
		ack.set_result(RET_LOGIN_WRONGPASS);

	SessionManager::getInstance()->sendProtoBuffer(sessionID, GameMsgID::MSG_ACK_LOGIN, ack);

	// 	int size = ack.ByteSize();
	// 	char msgData[SOCKET_BUFFER] = { 0, };
	// 
	// 	if (!ack.SerializePartialToArray(msgData, size))
	// 		return false;

	//SessionManager::getInstance()->sendBuffer(sessionID, GameMsgID::MSG_ACK_LOGIN, msgData, size);

	return true;
}

