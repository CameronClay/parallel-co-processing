#pragma once

#include <Includes.h>
#include <TCPServInterface.h>
#include <FileMappingIndexed.h>
#include <ThreadPool.h>
#include <MemPool.h>
#include "ClientQueue.h"
#include "ClientWorkMap.h"
#include "FileSend.h"
#include <GlobalNames.h>
#include <mutex>

class Server
{
public:
	Server(uint32_t nThreads = 2, uint64_t buffSize = 32KB);
	~Server();

	void Shutdown();

	TCPServInterface* GetTCPServ() const;
private:
	static const uint32_t MAXCLIENTS = 1000;

	friend static void MsgHandler(TCPServInterface& serv, ClientData* const clint, MsgStreamReader streamReader);
	friend static void ConnectHandler(TCPServInterface& serv, ClientData* data);
	friend static void DisconnectHandler(TCPServInterface& serv, ClientData* data, bool unexpected);
	void WorkThread();

	static bool VerifyNewData(const wchar_t* newFilename, const wchar_t* oldFilename);

	bool GiveNewWork(BuffAllocator* alloc, ClientData* clint);
	void GiveOldWork(BuffAllocator* alloc, ClientData* clint, const WorkInfo& wi);

	void OnCompletion();

	TCPServInterface* serv;
	ClientQueue clntQueue;
	ClientWorkMap workMap;
	FileSend fileSend;
	FileMappingIndexed tempFileMap;
	std::mutex fileMapLock, workLock;
	ThreadPool threadPool;
	std::atomic<uint32_t> reorderCounter;
	boost::lockfree::queue<WorkInfo, boost::lockfree::fixed_sized<true>> oldWork;
	TimePoint startTime;
	std::atomic<uint32_t> nReady;
	std::atomic<bool> threadInitialized, exitThread;
};                   