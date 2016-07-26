#include "Server.h"
#include <WaitableTimer.h>
#include <MessagesExt.h>
#include <DataInterp.h>
#include <Chrono>

void Server::WorkThread()
{
	WaitableTimer timer(true);
	timer.Set(DataInterp::ACCEPTEDTIME);

	while (timer.Wait())
	{
		if (exitThread.load(std::memory_order_acquire))
			return;

		ClientData* clint = nullptr;
		while (clint = clntQueue.FindClientForWork())
		{
			const uint32_t buffSize = DataInterp::GetChunkSize() + MSG_OFFSET;
			auto sndBuff = serv->GetSendBuffer(buffSize);
			*(short*)(sndBuff.buffer) = TYPE_WORK;
			*(short*)(sndBuff.buffer + 1) = MSG_WORK_NEW;

			DWORD64 cursor = 0;
			const uint32_t dataSize = DataInterp::GetClientWork(sndBuff.buffer + MSG_OFFSET, buffSize, cursor);

			workMap.Change(clint, cursor, dataSize);

			if (!serv->SendClientData(sndBuff, dataSize + MSG_OFFSET, clint, true))
				workMap.Remove(clint);

			if (exitThread.load(std::memory_order_acquire))
				return;
		}
	}
}

Server::Server()
	:
	serv(nullptr),
	fileSend(*serv),
	clntQueue(MAXCLIENTS),
	exitThread(false)
{
	fileSend.Initialize();
	serv = CreateServer(MsgHandler, ConnectHandler, DisconnectHandler, 5, BufferOptions(), SocketOptions(), 10, 30, 35, 15, 10, MAXCLIENTS, 30.0f, this);
	workThread = std::thread(&Server::WorkThread, this);
}
Server::~Server()
{
	DestroyServer(serv);
}

void Server::Shutdown()
{
	exitThread = true;
	workThread.join();
	serv->Shutdown();
}

TCPServInterface* Server::GetTCPServ() const
{
	return serv;
}

void MsgHandler(TCPServInterface& tcpServ, ClientData* const clint, MsgStreamReader streamReader)
{
	Server& serv = *(Server*)tcpServ.GetObj();
	const short type = streamReader.GetType(), msg = streamReader.GetMsg();
	switch (type)
	{
	case TYPE_WORK:
		switch (msg)
		{
			case MSG_WORK_NEW:
			{
				const float time = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - *(TimePoint*)clint->obj).count();

				if (!serv.clntQueue.EvaluateClient(clint, time))
				{
					tcpServ.SendMsg(clint, true, TYPE_KICK, MSG_KICK_TOOSLOW);
					tcpServ.DisconnectClient(clint);
				}
			}
			break;
		}
		break;
	case TYPE_READY:
		switch (msg)
		{
			case MSG_READY_PROCESS:
			{
				serv.clntQueue.AddClient(clint);
			}
			break;
		}
	}
}

void DisconnectHandler(TCPServInterface& tcpServ, ClientData* clint, bool unexpected)
{
	Server& serv = *(Server*)tcpServ.GetObj();
	serv.clntQueue.RemoveClient(clint);

	//Check if client still has work to be processed
	ClientWorkMap::Work work;
	if (serv.workMap.GetClientWork(clint, work))
	{
		//Client has work to be processed; give work to another client...
		ClientData* clint = serv.clntQueue.FindClientForWork();

		const uint32_t buffSize = work.dataSize + MSG_OFFSET;
		auto sndBuff = tcpServ.GetSendBuffer(buffSize);
		*(short*)(sndBuff.buffer) = TYPE_WORK;
		*(short*)(sndBuff.buffer + 1) = MSG_WORK_NEW;

		DWORD64 cursor = 0;
		const uint32_t dataSize = DataInterp::GetClientWorkPrev(sndBuff.buffer + MSG_OFFSET, buffSize, work.cursorPos);

		serv.workMap.Change(clint, cursor, dataSize);

		if (!tcpServ.SendClientData(sndBuff, buffSize, clint, true))
			serv.workMap.Remove(clint);
	}
}

void ConnectHandler(TCPServInterface& tcpServ, ClientData* clint)
{
	Server& serv = *(Server*)tcpServ.GetObj();

	//Transfer algorithim to client
	serv.fileSend.Wait();
	std::vector<FileMisc::FileData> vect = FileMisc::GetFileNameList(_T("Algorithims"), 0, false);
	serv.fileSend.SendFiles(clint, vect);
}