#include "Predef.h"
#include "WorldServer.h"
#include "SocketBuffer.h"

WorldServer * WorldServer::_instance = nullptr;

WorldServer::WorldServer()
{

}

WorldServer::~WorldServer()
{

}

void WorldServer::onMove(const SocketBuffer* socketbuffer, const USER_GUID& clientID)
{
	//flatbuffers::FlatBufferBuilder builder;
	//const uint8_t *data = builder.GetBufferPointer();
	//int len = builder.GetSize();

/*
	const FBPacket::movement *m = FBPacket::Getmovement(socketbuffer->buffer);
	uint64_t guid = m->guid();
	const FBPacket::vec2 *_pos = m->pos();

	Application::instance().logger().information("Move : %ld , %ld", (long)_pos->x(), (long)_pos->y());
*/
}