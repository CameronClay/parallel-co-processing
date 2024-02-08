#pragma once
#include <Includes.h>
#include <TCPClientInterface.h>
#include <GlobOps.h>
#include "FileReceive.h"

class Client
{
public:
	Client(uint32_t nClients, uint64_t buffSize = 32KB);
	~Client();

	bool Connect(const LIB_TCHAR* dest, const LIB_TCHAR* port, bool ipv6 = false, float timeOut = 5.0f);
	bool RecvServData(DWORD nThreads = 2, DWORD nConcThreads = 1);
	void Disconnect();
private:
	void SignalReady();

	const uint32_t nClients;
	std::unique_ptr<TCPClientInterface*[]> clints;
	FileReceive fileReceive;

	friend void MsgHandler(TCPClientInterface& client, MsgStreamReader streamReader);
	friend void DisconnectHandler(TCPClientInterface& client, bool unexpected);
};