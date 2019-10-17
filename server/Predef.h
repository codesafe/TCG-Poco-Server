
#ifndef PREDEF
#define PREDEF

#if defined(WIN32) || defined(_WIN64)
#define OS_WINDOWS
#elif defined(LINUX32) || defined(LINUX64)
#define OS_PLATFORM_LINUX
#endif

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
#include "Poco/Any.h"
#include "Poco/NamedTuple.h"

#include "Poco/Data/Session.h"
#include "Poco/Data/SessionPool.h"
#include "Poco/Data/PooledSessionImpl.h"

#include "Poco/Data/MySQL/Connector.h"
#include "Poco/Data/MySQL/MySQLException.h"
#include "Poco/Data/MySQL/Utility.h"


//#include "Poco/Data/Common.h"
//#include "Poco/Data/BLOB.h"
//#include "Poco/Data/StatementImpl.h"

// #include "Poco/Data/SQLite/Connector.h"
// #include "Poco/Data/SQLite/SQLiteException.h"
// #include "Poco/Data/ODBC/Connector.h"
// #include "Poco/Data/ODBC/ODBCException.h"


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
using Poco::Any;
using Poco::AnyCast;


using Poco::Data::SessionPool;
using Poco::Data::Session;

using namespace Poco::Data;
using namespace Poco::Data::Keywords;
using Poco::Data::MySQL::ConnectionException;
using Poco::Data::MySQL::Utility;
using Poco::Data::MySQL::StatementException;
using Poco::format;
using Poco::NotFoundException;
using Poco::Int32;
using Poco::Nullable;
using Poco::Tuple;
using Poco::NamedTuple;


// google protobuf
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
using namespace google;

// game protobuf
#include "./pb/packet_def.pb.h"
#include "./pb/packet.pb.h"

#define ACCEPT_THREAD_NUM		2
#define SERVER_PORT				9000

// Session
#define SOCKET_BUFFER	4096 * 2
#define USER_GUID		long
#define PLAYROOM_ID		long

// Data buffer poo;
#define DEFAULT_BUFFER_POOL		3000

// DataBase
#define MYSQL_USER "root"
#define MYSQL_PWD  "1111"
#define MYSQL_HOST "192.168.56.101"
#define MYSQL_PORT 3306
#define MYSQL_DB   "tcg-database"


typedef std::function<bool(const char* buf, const int buflen, const USER_GUID& sessionID)> NET_RECEIVE_FUNCTOR;
typedef std::shared_ptr<NET_RECEIVE_FUNCTOR> NET_RECEIVE_FUNCTOR_PTR;



#ifdef OS_WINDOWS

#ifdef _DEBUG
#pragma  comment(lib,"libprotobufd.lib")
//#pragma  comment(lib,"libmysqld.lib")		// 이것을 쓰면 오류남 (왜?? 모름)
#pragma  comment(lib,"libmysql.lib")
#else
#pragma  comment(lib,"libprotobuf.lib")
#pragma  comment(lib,"libmysql.lib")
#endif

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"Iphlpapi.lib")


#endif




#endif