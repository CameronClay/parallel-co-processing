#include <assert.h>
#include "FileTransfer.h"
#include "StreamExt2.h"
#include "MessagesExt.h"

void FileSend::SendThread()
{
	const UINT maxBuffSize = serv.GetBufferOptions().GetMaxDataBuffSize();
	while (threadState != FileSend::EXITING)
	{
		startEv.Wait();

		bool entered = false;
		if (threadState == FileSend::RUNNING)
		{
			entered = true;
			auto strm = serv.CreateOutStream(StreamWriter::SizeType(fileList, dir), TYPE_FILETRANSFER, MSG_FILETRANSFER_SEND);
			strm.Write(fileList);
			strm.Write(dir);
			serv.SendClientData(strm, clint, true);

			if (threadState == FileSend::RUNNING)
			{
				for (const auto& it : fileList)
				{
					File file;
					if (dir.empty())
						file.Open(it.fileName.c_str(), GENERIC_READ);
					else
						file.Open((dir + std::tstring(_T("\\")) + it.fileName).c_str(), GENERIC_READ);

					DWORD64 size = it.size;
					while ((threadState == FileSend::RUNNING) && size)
					{
						auto sndBuff = serv.GetSendBuffer(maxBuffSize);
						*((short*)sndBuff.buffer) = TYPE_FILETRANSFER;
						*((short*)sndBuff.buffer + 1) = MSG_FILETRANSFER_SEND;

						const DWORD read = file.Read((void*)(sndBuff.buffer + MSG_OFFSET), maxBuffSize - MSG_OFFSET);
						serv.SendClientData(sndBuff, read + MSG_OFFSET, clint, true);
						size -= read;
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

void FileSend::SetFileList(const std::tstring& dir, const std::vector<FileMisc::FileData>& fileList)
{
	this->fileList = fileList;
	this->dir = dir;
}

void FileSend::SendFiles(ClientData* clint)
{
	if (fileList.empty())
		return;

	this->clint = clint;
	threadState = RUNNING;
	startEv.Set();
}

void FileSend::Wait()
{
	finishedEv.Wait();
}

void FileSend::SendFilesAndWait(ClientData* clint)
{
	SendFiles(clint);
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
		streamReader.Read(dir);
		if (!dir.empty())
		{
			dir.append(_T("\\"));
			if (!FileMisc::Exists(dir.c_str()))
				FileMisc::CreateFolder(dir.c_str());
		}

		curFile = fileList.begin();

		if (dir.empty())
			file.Open(curFile->fileName.c_str(), GENERIC_WRITE);
		else
			file.Open((dir + curFile->fileName).c_str(), GENERIC_WRITE);

		if (AdvanceFile())
			return true;
	}
	else
	{
		const UINT size = streamReader.GetDataSize();
		file.Write(streamReader.GetData(), size);
		curFile->size -= size;

		if (AdvanceFile())
			return true;
	}
	return false;
}

bool FileReceive::AdvanceFile()
{
	while (!curFile->size)
	{
		if (curFile != fileList.end() - 1)
		{
			++curFile;
			file.ChangeDate(curFile->dateModified);
			file.Close();

			if (dir.empty())
				file.Open(curFile->fileName.c_str(), GENERIC_WRITE);
			else
				file.Open((dir + curFile->fileName).c_str(), GENERIC_WRITE);
		}
		else
		{
			OnCompletion();
			return true;
		}
	}
	return false;
}


void FileReceive::OnCompletion()
{
	fileList.clear();
	file.Close();
}


