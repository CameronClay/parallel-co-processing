#include "Server.h"
#include <WaitableTimer.h>
#include <MessagesExt.h>
#include <DataInterp.h>
#include <Chrono>
#include <Algorithm.h>

bool Server::VerifyNewData(const wchar_t* newFilename, const wchar_t* oldFilename)
{
	static const uint32_t buffSize = 4096;

	File newData{ newFilename, GENERIC_READ }, oldData{ oldFilename, GENERIC_READ };
	if (!(newData.IsOpen() && oldData.IsOpen()))
		return false;

	const uint32_t tempSize = Algorithm::GetOutSize(buffSize);
	auto newBuff = std::make_unique<char[]>(buffSize), oldBuff = std::make_unique<char[]>(buffSize), tempBuff = std::make_unique<char[]>(tempSize);

	DWORD read = 0;
	uint32_t size = 0;
	while (true)
	{
		bool eof = false;
		read = oldData.Read(oldBuff.get(), buffSize);
		if (!read)
			eof = true;
		else
			size = Algorithm::AlgorithmInOut(oldBuff.get(), buffSize, tempBuff.get(), tempSize);

		read = newData.Read(newBuff.get(), size ? size : 1);
		if (!read)
		{
			if (eof)
				return true;
			else
				return false;
		}

		if (memcmp(newBuff.get(), tempBuff.get(), size) != 0)
			return false;
	}
}

void Server::WorkThread()
{
	WaitableTimer timer(true);
	timer.Set(DataInterp::WAITTIME);

	while (timer.Wait())
	{
		if (exitThread.load(std::memory_order_acquire))
			return;

		ClientData* clint = nullptr;
		while (clint = clntQueue.FindClientForWork())
		{
			//If there is any old work to process, process that if not, give out new work
			WorkInfo wi;
			if (oldWork.pop(wi))
				GiveOldWork(clint, wi);
			else
				if (!GiveNewWork(clint))
					return;

			if (exitThread.load(std::memory_order_acquire))
				return;
		}
	}
}

Server::Server(uint32_t nThreads, uint64_t buffSize)
	:
	//																																			  NCOMPBUFF     NBUFF          MAXCON
	serv(CreateServer(MsgHandler, ConnectHandler, DisconnectHandler, 5, BufferOptions(buffSize, 1MB, 9, 32KB), SocketOptions(0, 0, true), 10, 30, MAXCLIENTS, MAXCLIENTS, 10, MAXCLIENTS, 30.0f, this)),
	fileSend(*serv, MAXCLIENTS),
	clntQueue(MAXCLIENTS),
	threadPool(nThreads),
	reorderCounter(0),
	oldWork(MAXCLIENTS),
	exitThread(false)
{
	DataInterp::LoadData(1, serv->GetBufferOptions().GetMaxDataSize() - MSG_OFFSET); //calculate exact amount of data that can be processed without allocating additional memory
	tempFileMap.Create(NEWDATANAME, Algorithm::GetOutSize(DataInterp::GetFileSize()), CREATE_ALWAYS);

	fileSend.Initialize(ALGORITHMPATH, FileMisc::GetFileNameList(ALGORITHMPATH, 0, false));
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

bool Server::GiveNewWork(ClientData* clint)
{
	const uint32_t chunkSize = DataInterp::GetChunkSize(), buffSize = chunkSize + MSG_OFFSET;
	auto sndBuff = serv->GetSendBuffer(buffSize);
	*((short*)sndBuff.buffer) = TYPE_WORK;
	*((short*)sndBuff.buffer + 1) = MSG_WORK_NEW;

	workLock.lock();
	WorkInfo wi = DataInterp::GetClientWork(sndBuff.buffer + MSG_OFFSET, chunkSize);
	workLock.unlock();

	if (wi.size)
	{
		workMap.Change(clint, wi);
		*static_cast<TimePoint*>(clint->obj) = Clock::now();

		if (!serv->SendClientData(sndBuff, wi.size + MSG_OFFSET, clint, true))
			workMap.Remove(clint);
	}
	else
	{
		serv->SendClientData(sndBuff, 0, nullptr, true); //free the buffer since it will no longer be used

		if (DataInterp::ORDERED && workMap.Empty() && oldWork.empty() && ++reorderCounter == 1)
		{
			_tprintf(_T("All data received; reordering...\n"));
			tempFileMap.ReorderFileData();

			_tprintf(_T("Data reordered; verifing...\n"));
			const bool verified = Server::VerifyNewData(NEWDATANAME, DATANAME);
			if (verified)
				_tprintf(_T("Data Verified\n"));
			else
				_tprintf(_T("Data not verified\n"));
		}

		return false;
	}
	return true;
}

void Server::GiveOldWork(ClientData* clint, const WorkInfo& wi)
{
	const uint32_t buffSize = wi.size + MSG_OFFSET;
	auto sndBuff = serv->GetSendBuffer(buffSize);
	*(short*)(sndBuff.buffer) = TYPE_WORK;
	*(short*)(sndBuff.buffer + 1) = MSG_WORK_NEW;

	workLock.lock();
	DataInterp::GetClientWorkPrev(sndBuff.buffer + MSG_OFFSET, wi.size, wi);
	workLock.unlock();

	workMap.Change(clint, wi);

	*static_cast<TimePoint*>(clint->obj) = Clock::now();

	if (serv->SendClientData(sndBuff, buffSize, clint, true))
		workMap.Remove(clint);
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
				serv.fileSend.SendFileList(clint);
			}
			break;
			case MSG_READY_PROCESS:
			{
				serv.clntQueue.AddClient(clint);
			}
			break;
		}
		break;
	case TYPE_FILETRANSFER:
		switch (msg)
		{
			case MSG_FILETRANSFER_SEND:
			{
				serv.fileSend.QueueSend(clint);
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

	//Check if client still has work to be processed, if so store for later processing
	WorkInfo wi;
	if (serv.workMap.GetClientWork(clint, wi))
		serv.oldWork.push(wi);
}

void ConnectHandler(TCPServInterface& tcpServ, ClientData* clint)
{
	Server& serv = *(Server*)tcpServ.GetObj();
}