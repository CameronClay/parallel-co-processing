#include "Server.h"
#include <WaitableTimer.h>
#include <MessagesExt.h>
#include <DataInterp.h>
#include <Chrono>
#include <Algorithm.h>

void Server::WorkThread()
{
	WaitableTimer timer(true);
	timer.Set(DataInterp::WAITTIME);

	const uint32_t chunkSize = DataInterp::GetChunkSize(), buffSize = chunkSize + MSG_OFFSET;

	while (timer.Wait())
	{
		if (exitThread.load(std::memory_order_acquire))
			return;

		ClientData* clint = nullptr;
		while (clint = clntQueue.FindClientForWork())
		{
			auto sndBuff = serv->GetSendBuffer(buffSize);
			*((short*)sndBuff.buffer) = TYPE_WORK;
			*((short*)sndBuff.buffer + 1) = MSG_WORK_NEW;

			workLock.lock();
			WorkInfo wi = DataInterp::GetClientWork(sndBuff.buffer + MSG_OFFSET, chunkSize);
			workLock.unlock();

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

				if(DataInterp::ORDERED && workMap.Empty() && ++reorderCounter == 1)
					tempFileMap.ReorderFileData();
				return;
			}

			if (exitThread.load(std::memory_order_acquire))
				return;
		}
	}
}

Server::Server(uint32_t nThreads, uint64_t buffSize)
	:
	serv(CreateServer(MsgHandler, ConnectHandler, DisconnectHandler, 5, BufferOptions(buffSize, 1MB), SocketOptions(0, 0, true), 10, 30, MAXCLIENTS, 15, 10, MAXCLIENTS, 30.0f, this)),
	fileSend(*serv),
	clntQueue(MAXCLIENTS),
	threadPool(nThreads),
	reorderCounter(0),
	exitThread(false)
{
	DataInterp::LoadData(1, serv->GetBufferOptions().GetMaxDataSize() - MSG_OFFSET); //calculate exact amount of data that can be processed without allocating additional memory
	tempFileMap.Create(_T("NewData.dat"), Algorithm::GetOutSize(DataInterp::GetFileSize()), CREATE_ALWAYS);

	//FileMisc::SetCurDirectory(L"Algorithms");
	fileSend.Initialize();
	threadPool.Initialize(&Server::WorkThread, this);
}
Server::~Server()
{
	DestroyServer(serv);
}

void Server::Shutdown()
{
	exitThread = true;
	threadPool.Wait();
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

				WorkInfo wi;
				if (serv.workMap.GetClientWork(clint, wi))
				{
					serv.fileMapLock.lock();
					serv.tempFileMap.Write(wi.curIndex, streamReader.GetData(), streamReader.GetDataSize());
					serv.fileMapLock.unlock();

					if (!serv.clntQueue.EvaluateClient(clint, time))
					{
						tcpServ.SendMsg(clint, true, TYPE_KICK, MSG_KICK_TOOSLOW);
						tcpServ.DisconnectClient(clint);
					}
				}
				else
				{
					tcpServ.DisconnectClient(clint);
				}
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
		ClientData* client = nullptr;
		do
		{
			client = serv.clntQueue.FindClientForWork();
		} while (!client);

		const uint32_t buffSize = wi.size + MSG_OFFSET;
		auto sndBuff = tcpServ.GetSendBuffer(buffSize);
		*(short*)(sndBuff.buffer) = TYPE_WORK;
		*(short*)(sndBuff.buffer + 1) = MSG_WORK_NEW;

		DWORD64 cursor = 0;

		serv.workLock.lock();
		DataInterp::GetClientWorkPrev(sndBuff.buffer + MSG_OFFSET, buffSize, wi);
		serv.workLock.unlock();

		serv.workMap.Change(client, wi);

		*(TimePoint*)(client->obj) = Clock::now();

		if (!tcpServ.SendClientData(sndBuff, buffSize, client, true))
			serv.workMap.Remove(client);
	}
}

void ConnectHandler(TCPServInterface& tcpServ, ClientData* clint)
{
	Server& serv = *(Server*)tcpServ.GetObj();
}