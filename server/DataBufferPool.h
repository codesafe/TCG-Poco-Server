#ifndef DATABUFFER_POOL
#define DATABUFFER_POOL

#include "Predef.h"

#define USE_STL

class DataBuff
{
public:
	DataBuff();
	~DataBuff();

	void	flush(int size);
	bool	copyData(char *data, int size);
	void	release();


	int		buffersize;
	char	buffer[SOCKET_BUFFER];

#ifndef USE_STL
	DataBuff *	prev;
	DataBuff *	next;
#endif
};


//////////////////////////////////////////////////////////////////////////


#ifdef USE_STL

class DataBufferPool
{
public:
	DataBufferPool();
	~DataBufferPool();

	bool		init(int poolnum);
	DataBuff*	allocBuffer();
	bool		releaseBuffer(DataBuff *buff);

private:
	std::deque<DataBuff *>	bufferpool;
	std::mutex	bufferlock;
};


#else

class DataBufferPool
{
public:
	DataBufferPool();
	~DataBufferPool();

	BOOL		init(int poolnum);
	DataBuff*	allocBuffer();
	BOOL		releaseBuffer(DataBuff *buff);

private:
	DataBuff * head;
	//DataBuff * tail;

	std::mutex	bufferlock;
};

#endif

//////////////////////////////////////////////////////////////////////////

// size 별로 준비??

class DataBufferManager
{
private:
	DataBufferManager();
	~DataBufferManager();

	static DataBufferManager * _instance;

	DataBufferPool	databufferpool;

public:

	static DataBufferManager * getInstance()
	{
		if (_instance == nullptr)
			_instance = new DataBufferManager();
		return _instance;
	}

	DataBuff* allocBuffer(int size);
	bool	releaseBuffer(DataBuff *buff);
};


#endif