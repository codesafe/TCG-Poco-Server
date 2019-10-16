#ifndef PACKET
#define PACKET

#include "Predef.h"

// #define PROCESS_MESSAGE(msgID, Func) \
// 		case msgID:{\
// 		if(Func(buf, buflen, guid)){return TRUE;}} break;

#define CLIENT_SIG 0xC0
#define SERVER_SIG 0xDE

#pragma  pack(push)
#pragma  pack(1)


struct PacketHeader
{
	unsigned char	signature;		// ������ ������?(SERVER_SIG) Ŭ���̾�Ʈ�� ������? (CLIENT_SIG)
	int		packetsize;		// packet size�� header + data ������ ��ü ����
	int		packetserial;	// ���� ��ȣ = wCommandID^dwSize+index(��Ű���� �ڵ� ���� ����); ȯ�� ��ȣ = pHeader->dwPacketNo - pHeader->wCommandID^pHeader->dwSize;
	int		packetID;			// msg ID
};

#pragma  pack(pop)


struct Packet_LOGIN
{
	USER_GUID	gsn;
	std::string name;
	std::string password;

	Packet_LOGIN() { gsn = 0; }
	Packet_LOGIN(const USER_GUID& ln, const std::string& fn, const std::string& adr)
	{
		gsn = ln;
		name = fn;
		password = adr;
	}
};


template <>
class TypeHandler<Packet_LOGIN>
{
public:
// 	static void bind(std::size_t pos, const Packet_LOGIN& obj, AbstractBinder::Ptr pBinder, AbstractBinder::Direction dir)
// 	{
// 		poco_assert_dbg(!pBinder.isNull());
// 		pBinder->bind(pos++, obj.gsn, dir);
// 		pBinder->bind(pos++, obj.name, dir);
// 		pBinder->bind(pos++, obj.password, dir);
// 	}

	static void prepare(std::size_t pos, const Packet_LOGIN& obj, AbstractPreparator::Ptr pPrepare)
	{
// 		poco_assert_dbg(!pPrepare.isNull());
// 		pPrepare->prepare(pos++, obj.gsn);
// 		pPrepare->prepare(pos++, obj.name);
// 		pPrepare->prepare(pos++, obj.password);
	}

	static std::size_t size()
	{
		return 3;
	}

	static void extract(std::size_t pos, Packet_LOGIN& obj, const Packet_LOGIN& defVal, AbstractExtractor::Ptr pExt)
	{
		poco_assert_dbg(!pExt.isNull());
		if (!pExt->extract(pos++, obj.gsn))
			obj.gsn = defVal.gsn;
		if (!pExt->extract(pos++, obj.name))
			obj.name = defVal.name;
		if (!pExt->extract(pos++, obj.password))
			obj.password = defVal.password;
	}

private:
	TypeHandler();
//	~TypeHandler();
	TypeHandler(const TypeHandler&);
//	TypeHandler& operator=(const TypeHandler&);
};


#endif
