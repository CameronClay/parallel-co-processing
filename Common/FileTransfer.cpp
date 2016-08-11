#include <assert.h>
#include "FileTransfer.h"
#include "StreamExt2.h"
#include "MessagesExt.h"

void FileSend::SendThread()
{
	while (threadState != FileSend::EXITING)
	{
		startEv.Wait();

		bool entered = false;
		if (threadState == FileSend::RUNNING)
		{
			entered = true;
			auto strm = serv.CreateOutStream(StreamWriter::SizeType(fileList), TYPE_FILETRANSFER, MSG_FILETRANSFER_SEND); //sizetype = 58, strm size = 62 (MSG_OFFSET + 58)
			strm.Write(fileList);
			serv.SendClientData(strm, clint, true);

			if (threadState == FileSend::RUNNING)
			{
				const UINT maxBuffSize = serv.GetBufferOptions().GetMaxDataBuffSize();
				for (auto& it : fileList)
				{
					File file((std::wstring(L"Algorithms\\") + it.fileName).c_str(), GENERIC_READ);

					while ((threadState == FileSend::RUNNING) && it.size)
					{
						auto sndBuff = serv.GetSendBuffer(maxBuffSize);
						*((short*)sndBuff.buffer) = TYPE_FILETRANSFER;
						*((short*)sndBuff.buffer + 1) = MSG_FILETRANSFER_SEND;

						DWORD read = file.Read((void*)(sndBuff.buffer + MSG_OFFSET), maxBuffSize - MSG_OFFSET);
						serv.SendClientData(sndBuff, read + MSG_OFFSET, clint, true);
						it.size -= read;
					}
				}
			}
		}

		if (entered && (threadState == FileSend::ABORTING))
			serv.SendMsg(clint, true, TYPE_FILETRANSFER, MSG_FILETRANSFER_ABORTED);

		finishedEv.Set();
	}
}

FileSend::FileSend(TCPServInterface& serv)
	:
	serv(serv),
	threadState(RUNNING)
{
	finishedEv.Set();
}
FileSend::~FileSend()
{}

void FileSend::Initialize()
{
	thread = std::thread(&FileSend::SendThread, this);
}

void FileSend::SendFiles(ClientData* clint, const std::vector<FileMisc::FileData>& fileList)
{
	if (fileList.empty())
		return;

	this->clint = clint;
	this->fileList = fileList;
	threadState = RUNNING;
	startEv.Set();
}

void FileSend::Wait()
{
	finishedEv.Wait();
}

void FileSend::SendFilesAndWait(ClientData* clint, const std::vector<FileMisc::FileData>& fileList)
{
	SendFiles(clint, fileList);
	Wait();
}

void FileSend::StopSend()
{
	threadState = ABORTING;
	Wait();
}


FileReceive::FileReceive()
{}

FileReceive::~FileReceive()
{}

bool FileReceive::ReadFiles(MsgStreamReader& streamReader)
{
	if (fileList.empty())
	{
		streamReader.Read(fileList);
		curFile = fileList.begin();
	}
	else
	{
		while (!curFile->size)
		{
			if (curFile != fileList.end() - 1)
			{
				++curFile;
				file.Close();
				file.Open(curFile->fileName.c_str(), GENERIC_WRITE);
			}
			else
			{
				OnCompletion();
				return true;
			}
		}

		const UINT size = streamReader.GetDataSize();
		file.Write(streamReader.GetData(), size);
		curFile->size -= size;
		while (curFile->size == 0)
		{
			if (curFile != fileList.end() - 1)
			{
				++curFile;
				file.Close();
				file.Open(curFile->fileName.c_str(), GENERIC_WRITE);
			}
			else
			{
				OnCompletion();
				return true;
			}
		}
	}
	return false;
}

void FileReceive::OnCompletion()
{
	fileList.clear();
	file.Close();
}


