#include "DataBufferPool.h"

DataBuff::DataBuff()
{
	buffersize = 0;
#ifndef USE_STL
	prev = nullptr;
	next = nullptr;
#endif
}

DataBuff::~DataBuff()
{
}

void DataBuff::flush(int size)
{
	if (buffersize > size)
	{
#ifdef _DEBUG
		char temp[SOCKET_BUFFER];
		memcpy(temp, buffer + size, buffersize - size);
		memset(buffer, 0, SOCKET_BUFFER);
		memcpy(buffer, temp, buffersize - size);
#else
		// 나머지를 앞쪽으로 밀어 정리한다
		memmove(buffer, buffer + size, buffersize - size);
#endif

		buffersize -= size;
	}
	else
		buffersize = 0;
}

bool DataBuff::copyData(char *data, int size)
{
	if (buffersize + size > SOCKET_BUFFER) 
		return false;

	memcpy(buffer + buffersize, data, size);
	buffersize += size;
	return true;
}

void DataBuff::release()
{
	buffersize = 0;
}

//////////////////////////////////////////////////////////////////////////

#ifdef USE_STL

DataBufferPool::DataBufferPool()
{
}

DataBufferPool::~DataBufferPool()
{
}

bool DataBufferPool::init(int poolnum)
{
	for (int i = 0; i < poolnum; i++)
	{
		DataBuff *buff = new DataBuff();
		bufferpool.push_back(buff);
	}

	return true;
}

// 버퍼 얻을때에는 head부터 얻는다
DataBuff *DataBufferPool::allocBuffer()
{
	DataBuff *ret = nullptr;

	bufferlock.lock();

	if (bufferpool.empty() == false)
	{
		ret = bufferpool[0];
		bufferpool.pop_front();
	}
	else
	{
		// 모자르다 더 만들어야 한다.
		ret = new DataBuff();
	}

	bufferlock.unlock();

	return ret;
}

// 해제시에는 head 앞에 추가
bool DataBufferPool::releaseBuffer(DataBuff *buff)
{
	bufferlock.lock();
	buff->release();
	bufferpool.push_front(buff);
	bufferlock.unlock();
	return true;
}


#else

DataBufferPool::DataBufferPool()
{
	head = nullptr;
	//tail = nullptr;
}

DataBufferPool::~DataBufferPool()
{

}


bool DataBufferPool::init(int poolnum)
{
	DataBuff *prevbuf = nullptr;
	for (int i = 0; i < poolnum; i++)
	{
		DataBuff *buff = new DataBuff();
		if (head == nullptr)
			head = buff;
		else
		{
			prevbuf->next = buff;
			buff->prev = prevbuf;
		}

		prevbuf = buff;
	}
	//tail = prevbuf;
	return true;
}

// 버퍼 얻을때에는 head부터 얻는다
DataBuff *DataBufferPool::allocBuffer()
{
	DataBuff *ret = nullptr;

	bufferlock.lock();
	if (head != nullptr)
	{
		ret = head;
		head = head->next;
		head->prev = nullptr;

		ret->next = nullptr;
		ret->prev = nullptr;
	}
	else
	{
		// 모자르다 더 만들어야 한다.
		ret = new DataBuff();
	}
	bufferlock.unlock();

	return ret;
}

// 해제시에는 head 앞에 추가
bool DataBufferPool::releaseBuffer(DataBuff *buff)
{
	bufferlock.lock();
	if (head == nullptr)
	{
		head = buff;
	}
	else
	{
		head->prev = buff;
		buff->next = head;
		head = buff;
	}
	bufferlock.unlock();
	return true;
}

#endif

//////////////////////////////////////////////////////////////////////////

DataBufferManager * DataBufferManager::_instance = nullptr;

DataBufferManager::DataBufferManager()
{
	databufferpool.init(DEFAULT_BUFFER_POOL);
}

DataBufferManager::~DataBufferManager()
{

}

DataBuff* DataBufferManager::allocBuffer(int size)
{
	// size 별로 준비해서 꺼내??
	return databufferpool.allocBuffer();
}

bool DataBufferManager::releaseBuffer(DataBuff *buff)
{
	return databufferpool.releaseBuffer(buff);
}