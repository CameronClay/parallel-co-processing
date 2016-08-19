#include "FileSend.h"
#include "StreamExt2.h"
#include "MessagesExt.h"
#include <WaitableTimer.h>
#include <DataInterp.h>

void FileSend::SendThread()
{
	const UINT maxBuffSize = serv.GetBufferOptions().GetMaxDataBuffSize();

	WaitableTimer timer(true);
	timer.Set(DataInterp::WAITTIME);

	while (timer.Wait())
	{
		ClientData* clint = nullptr;
		while (clientQueue.pop(clint))
		{
			if (threadState.load(std::memory_order_acquire) == FileSend::RUNNING)
			{
				if (!SendFiles(clint, maxBuffSize))
					serv.SendMsg(clint, true, TYPE_FILETRANSFER, MSG_FILETRANSFER_ABORTED);
			}
		}
	}
}

FileSend::FileSend(TCPServInterface& serv, uint32_t maxClients)
	:
	serv(serv),
	clientQueue(maxClients),
	threadState(FileSend::RUNNING)
{}
FileSend::~FileSend()
{
	StopSend();
}

void FileSend::Initialize(const std::tstring& dir, const std::vector<FileMisc::FileData>& fileList)
{
	this->dir = dir;
	this->fileList = fileList;
	fileListSize = StreamWriter::SizeType(fileList, dir);

	thread = std::thread(&FileSend::SendThread, this);
}

void FileSend::QueueSend(ClientData* clint)
{
	clientQueue.push(clint);
}

void FileSend::StopSend()
{
	threadState = EXITING;
	if (thread.joinable())
		thread.join();
}

bool FileSend::SendFileList(ClientData* clint)
{
	auto strm = serv.CreateOutStream(fileListSize, TYPE_FILETRANSFER, MSG_FILETRANSFER_LIST);
	strm.Write(fileList);
	strm.Write(dir);
	if (!serv.SendClientData(strm, clint, true))
		return false;

	return true;
}

bool FileSend::SendFiles(ClientData* clint, uint32_t maxBuffSize)
{
	for (const auto& it : fileList)
	{
		File file;
		if (dir.empty())
			file.Open(it.fileName.c_str(), GENERIC_READ);
		else
			file.Open((dir + std::tstring(_T("\\")) + it.fileName).c_str(), GENERIC_READ);

		DWORD64 size = it.size;
		while ((threadState.load(std::memory_order_acquire) == FileSend::RUNNING) && size)
		{
			auto sndBuff = serv.GetSendBuffer(maxBuffSize);
			*(reinterpret_cast<short*>(sndBuff.buffer)) = TYPE_FILETRANSFER;
			*(reinterpret_cast<short*>(sndBuff.buffer) + 1) = MSG_FILETRANSFER_SEND;

			const DWORD read = file.Read(static_cast<void*>(sndBuff.buffer + MSG_OFFSET), maxBuffSize - MSG_OFFSET);
			serv.SendClientData(sndBuff, read + MSG_OFFSET, clint, true);
			size -= read;
		}

		if (threadState.load(std::memory_order_acquire) != FileSend::RUNNING)
			return false;
	}

	return true;
}
