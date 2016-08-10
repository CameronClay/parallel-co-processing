#pragma once

#include <Includes.h>
#include <TCPServInterface.h>
#include <FileTransfer.h>
#include <FileMappingIndexed.h>
#include <MemPool.h>
#include "ClientQueue.h"
#include "ClientWorkMap.h"

class Server
{
public:
	Server();
	~Server();

	void Shutdown();

	TCPServInterface* GetTCPServ() const;
private:
	static const uint32_t MAXCLIENTS = 1000;

	friend static void MsgHandler(TCPServInterface& serv, ClientData* const clint, MsgStreamReader streamReader);
	friend static void ConnectHandler(TCPServInterface& serv, ClientData* data);
	friend static void DisconnectHandler(TCPServInterface& serv, ClientData* data, bool unexpected);
	void WorkThread();

	TCPServInterface* serv;
	ClientQueue clntQueue;
	ClientWorkMap workMap;
	FileSend fileSend;
	FileMappingIndexed tempFileMap;
	MemPoolSync<> timePool;
	std::thread workThread;
	std::atomic<bool> exitThread;
};                   