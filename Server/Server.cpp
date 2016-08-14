#include "Server.h"
#include <WaitableTimer.h>
#include <MessagesExt.h>
#include <DataInterp.h>
#include <Chrono>
#include <Algorithm.h>

void Server::WorkThread()
{
	WaitableTimer timer(true);
	timer.Set(DataInterp::ACCEPTEDTIME);

	while (timer.Wait())
	{
		if (exitThread.load(std::memory_order_acquire))
			return;

		ClientData* clint = nullptr;
		const uint32_t buffSize = DataInterp::GetChunkSize() + MSG_OFFSET;
		while (clint = clntQueue.FindClientForWork())
		{
			auto sndBuff = serv->GetSendBuffer(buffSize);
			*((short*)sndBuff.buffer) = TYPE_WORK;
			*((short*)sndBuff.buffer + 1) = MSG_WORK_NEW;

			DWORD64 cursor = 0;
			WorkInfo wi = DataInterp::GetClientWork(sndBuff.buffer + MSG_OFFSET, buffSize - MSG_OFFSET); //needs sync if multiple threads
			if (wi.size)
			{
				workMap.Change(clint, wi);

				*(TimePoint*)(clint->obj) = Clock::now();

				if (!serv->SendClientData(sndBuff, wi.size + MSG_OFFSET, clint, true))
					workMap.Remove(clint);
			}
			else
			{
				serv->SendClientData(sndBuff, 0, nullptr, true); //free the buffer since it will no longer be used
				tempFileMap.ReorderFileData();
				return;
			}

			if (exitThread.load(std::memory_order_acquire))
				return;
		}
	}
}

Server::Server()
	:
	serv(CreateServer(MsgHandler, ConnectHandler, DisconnectHandler, 5, BufferOptions(8KB, 2MB), SocketOptions(), 10, 30, 35, 15, 10, MAXCLIENTS, 30.0f, this)),
	fileSend(*serv),
	clntQueue(MAXCLIENTS),
	exitThread(false)
{
	DataInterp::LoadData(1, serv->GetBufferOptions().GetMaxDataSize() - MSG_OFFSET); //calculate exact amount of data that can be processed without allocating additional memory
	tempFileMap.Create(_T("NewData.dat"), Algorithm::GetOutSize(DataInterp::GetFileSize()));

	//FileMisc::SetCurDirectory(L"Algorithms");
	fileSend.Initialize();
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
			case MSG_WORK_COMPLETE:
			{
				const float time = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - *(TimePoint*)clint->obj).count();

				if (!serv.clntQueue.EvaluateClient(clint, time))
				{
					tcpServ.SendMsg(clint, true, TYPE_KICK, MSG_KICK_TOOSLOW);
					tcpServ.DisconnectClient(clint);
				}

				WorkInfo wi;
				if (serv.workMap.GetClientWork(clint, wi))
					serv.tempFileMap.Write(wi.curIndex, streamReader.GetData(), streamReader.GetDataSize()); // needs sync
				else
					tcpServ.DisconnectClient(clint);
			}
			break;
		}
		break;
	case TYPE_READY:
		switch (msg)
		{
			case MSG_READY_INITIALIZED:
			{
				//Transfer algorithm to client
				serv.fileSend.Wait();
				auto vect = FileMisc::GetFileNameList(L"Algorithms", 0, false);
				serv.fileSend.SendFiles(clint, vect);
			}
			break;
			case MSG_READY_PROCESS:
			{
				serv.clntQueue.AddClient(clint);
			}
			break;
		}
		break;
	}
}

void DisconnectHandler(TCPServInterface& tcpServ, ClientData* clint, bool unexpected)
{
	Server& serv = *(Server*)tcpServ.GetObj();
	serv.clntQueue.RemoveClient(clint);

	//Check if client still has work to be processed
	WorkInfo wi;
	if (serv.workMap.GetClientWork(clint, wi))
	{
		//Client has work to be processed; give work to another client...
		ClientData* clint = serv.clntQueue.FindClientForWork();

		const uint32_t buffSize = wi.size + MSG_OFFSET;
		auto sndBuff = tcpServ.GetSendBuffer(buffSize);
		*(short*)(sndBuff.buffer) = TYPE_WORK;
		*(short*)(sndBuff.buffer + 1) = MSG_WORK_NEW;

		DWORD64 cursor = 0;
		DataInterp::GetClientWorkPrev(sndBuff.buffer + MSG_OFFSET, buffSize, wi);

		serv.workMap.Change(clint, wi);

		*(TimePoint*)(clint->obj) = Clock::now();

		if (!tcpServ.SendClientData(sndBuff, buffSize, clint, true))
			serv.workMap.Remove(clint);
	}
}

void ConnectHandler(TCPServInterface& tcpServ, ClientData* clint)
{
	Server& serv = *(Server*)tcpServ.GetObj();
}