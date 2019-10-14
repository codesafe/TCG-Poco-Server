
#ifndef PREDEF
#define PREDEF

#define POCO_STATIC

#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/ParallelSocketReactor.h"

#include "Poco/Net/SocketAcceptor.h"
#include "Poco/Net/ParallelSocketAcceptor.h"

#include "Poco/Net/SocketNotification.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/NObserver.h"
#include "Poco/Exception.h"
#include "Poco/Thread.h"
#include "Poco/FIFOBuffer.h"
#include "Poco/Delegate.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/ThreadPool.h"
#include "Poco/RWLock.h"
#include "Poco/HashMap.h"

#include "Poco/Redis/AsyncReader.h"
#include "Poco/Redis/Command.h"
#include "Poco/Redis/PoolableConnectionFactory.h"

#include <iostream>
#include <string>
#include <sstream>

#include <vector>
#include <list>
#include <algorithm>
#include <assert.h>
#include <set>
#include <deque>
#include <mutex>
#include <thread>

using Poco::Net::SocketReactor;
using Poco::Net::SocketAcceptor;
using Poco::Net::ParallelSocketAcceptor;

using Poco::Net::ReadableNotification;
using Poco::Net::WritableNotification;
using Poco::Net::ShutdownNotification;
using Poco::Net::ErrorNotification;
using Poco::Net::IdleNotification;
using Poco::Net::TimeoutNotification;

using Poco::Net::ServerSocket;
using Poco::Net::StreamSocket;

using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;

using Poco::Timespan;
using Poco::NObserver;
using Poco::AutoPtr;
using Poco::Thread;
using Poco::FIFOBuffer;
using Poco::delegate;
using Poco::Observer;

// google protobuf
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
using namespace google;

// game protobuf
#include "./pb/packet_def.pb.h"
#include "./pb/packet.pb.h"


#define SOCKET_BUFFER	4096 * 2
#define USER_GUID		long


typedef std::function<bool(const char* buf, const int buflen, const USER_GUID& sessionID)> NET_RECEIVE_FUNCTOR;
typedef std::shared_ptr<NET_RECEIVE_FUNCTOR> NET_RECEIVE_FUNCTOR_PTR;


#ifdef _DEBUG
	#pragma  comment(lib,"libprotobufd.lib")
	//#pragma  comment(lib,"mysqlpp_d.lib")
#else
	#pragma  comment(lib,"libprotobuf.lib")
	//#pragma  comment(lib,"mysqlpp.lib")
#endif


#endif