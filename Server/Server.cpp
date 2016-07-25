#include "Server.h"
#include <Chrono>

Server::Server()
	:
	serv(nullptr),
	fileSend(*serv),
	clntQueue(MAXCLIENTS)
{
	fileSend.Initialize();
	serv = CreateServer(MsgHandler, ConnectHandler, DisconnectHandler, 5, BufferOptions(), SocketOptions(), 10, 30, 35, 15, 10, MAXCLIENTS, 30.0f, this);
}
Server::~Server()
{
	DestroyServer(serv);
}

void Server::Shutdown()
{
	serv->Shutdown();
}

TCPServInterface* Server::GetTCPServ() const
{
	return serv;
}

void MsgHandler(TCPServInterface& tcpServ, ClientData* const clint, MsgStreamReader streamReader)
{
	Server& serv = *(Server*)tcpServ.GetObj();
	const float time = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - *(TimePoint*)clint->obj).count();
	serv.clntQueue.EvaluateClient(clint, time);

	//Give more work to client?
}

void DisconnectHandler(TCPServInterface& tcpServ, ClientData* clint, bool unexpected)
{
	Server& serv = *(Server*)tcpServ.GetObj();
	serv.clntQueue.RemoveClient(clint);
}

void ConnectHandler(TCPServInterface& tcpServ, ClientData* clint)
{
	Server& serv = *(Server*)tcpServ.GetObj();
	serv.clntQueue.AddClient(clint);

	//Transfer algorithim to client
	serv.fileSend.Wait();
	std::vector<FileMisc::FileData> vect = FileMisc::GetFileNameList(_T("Algorithims"), 0, false);
	serv.fileSend.SendFiles(clint, vect);
}