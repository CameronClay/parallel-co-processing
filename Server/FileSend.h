#pragma once

#include <Includes.h>
#include <TCPServInterface.h>
#include <MsgStream.h>
#include <File.h>
#include "Event.h"
#include <vector>
#include <thread>
#include <boost/lockfree/queue.hpp>

class FileSend
{
public:
	FileSend(TCPServInterface& serv, uint32_t maxClients);
	~FileSend();

	void Initialize(const std::tstring& dir, const std::vector<FileMisc::FileData>& fileList);

	void QueueSend(ClientData* clint);
	void StopSend();
private:
	enum ThreadState : uint32_t
	{
		RUNNING,
		EXITING
	};
	void SendThread();

	TCPServInterface& serv;
	std::tstring dir;
	std::vector<FileMisc::FileData> fileList;
	std::thread thread;
	std::atomic<uint32_t> threadState;
	boost::lockfree::queue<ClientData*, boost::lockfree::fixed_sized<true>> clientQueue;
};
