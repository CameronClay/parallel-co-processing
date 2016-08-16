#pragma once
#include <Includes.h>
#include <TCPClientInterface.h>
#include <FileTransfer.h>
#include <GlobOps.h>
//#include "AlgorithmAccess.h"

class Client
{
public:
	Client(uint64_t buffSize = 32KB);
	~Client();

	TCPClientInterface* GetTCPClient() const;
private:
	TCPClientInterface* clint;
	FileReceive fileReceive;
	//AlgorithmAccess algorithmAccess;

	friend static void MsgHandler(TCPClientInterface& client, MsgStreamReader streamReader);
	friend static void DisconnectHandler(TCPClientInterface& client, bool unexpected);
};