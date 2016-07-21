#include <assert.h>
#include "FileTransfer.h"
#include "StreamExt2.h"
#include "MessagesExt.h"

void SendThread(FileSend& fileSend)
{
	while (true)
	{
		TCPServInterface* serv = fileSend.serv;

		fileSend.startEv.Wait();

		if (fileSend.threadState == FileSend::RUNNING)
		{
			auto strm = serv->CreateOutStream(StreamWriter::SizeType(fileSend.fileList), TYPE_FILETRANSFER, MSG_FILETRANSFER_SEND);
			strm.Write(fileSend.fileList);
			serv->SendClientData(strm, fileSend.clint, true);

			if (fileSend.threadState == FileSend::RUNNING)
			{
				const UINT maxBuffSize = serv->GetBufferOptions().GetMaxDataBuffSize();
				for (auto& it : fileSend.fileList)
				{
					File file(it.fileName.c_str(), GENERIC_READ);
					DWORD64 totalRead = 0;

					while ((fileSend.threadState == FileSend::RUNNING) && (totalRead < it.size))
					{
						auto sndBuff = serv->GetSendBuffer(maxBuffSize);
						*(short*)(sndBuff.buffer) = TYPE_FILETRANSFER;
						*(short*)(sndBuff.buffer + 1) = MSG_FILETRANSFER_SEND;

						DWORD read = file.Read((void*)(sndBuff.buffer + MSG_OFFSET), maxBuffSize);
						serv->SendClientData(sndBuff, read, fileSend.clint, true);
						totalRead += read;
					}
				}
			}
		}

		if (fileSend.threadState == FileSend::EXITING)
		{
			serv->SendMsg(fileSend.clint, true, TYPE_FILETRANSFER, MSG_FILETRANSFER_ABORTED);
			fileSend.finishedEv.Set();
			return;
		}

		fileSend.finishedEv.Set();
	}
}

FileSend::FileSend()
	:
	thread(SendThread, std::ref(*this)),
	threadState(RUNNING)
{}
FileSend::~FileSend()
{

}

void FileSend::SendFiles(TCPServInterface* serv, ClientData* clint, const std::vector<FileMisc::FileData>& fileList)
{
	if (fileList.empty())
		return;

	this->serv = serv;
	this->clint = clint;
	this->fileList = fileList;
	threadState = RUNNING;
	startEv.Set();
}

void FileSend::Wait()
{
	finishedEv.Wait();
}

void FileSend::SendFilesAndWait(TCPServInterface* serv, ClientData* clint, const std::vector<FileMisc::FileData>& fileList)
{
	SendFiles(serv, clint, fileList);
	Wait();
}

void FileSend::StopSend()
{
	threadState = ABORTING;
	Wait();
}


FileReceive::FileReceive()
{

}
FileReceive::~FileReceive()
{

}

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
		totalRead += size;
		if (totalRead == curFile->size)
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


