#pragma once
#include <Includes.h>
#include <TCPClientInterface.h>
#include <FileTransfer.h>

class Client
{
public:
	Client();
	~Client();

	TCPClientInterface* GetTCPClient() const;
private:
	TCPClientInterface* clint;
	FileReceive fileReceive;

	friend static void MsgHandler(TCPClientInterface& client, MsgStreamReader streamReader);
	friend static void DisconnectHandler(TCPClientInterface& client, bool unexpected);
};