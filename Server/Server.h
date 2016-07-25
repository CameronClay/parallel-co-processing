#pragma once

#include <Includes.h>
#include <TCPServInterface.h>
#include <FileTransfer.h>
#include "ClientQueue.h"

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

	TCPServInterface* serv;
	ClientQueue clntQueue;
	FileSend fileSend;
};                   