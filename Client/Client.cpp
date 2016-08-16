#include "Client.h"
#include <Algorithm.h>
#include <MessagesExt.h>
#include <GlobOps.h>
#include <MsgHeader.h>

Client::Client(uint64_t buffSize)
{
	const uint32_t outSizeMax = Algorithm::GetOutSize(buffSize);
	clint = CreateClient(MsgHandler, DisconnectHandler, 5, BufferOptions(outSizeMax + sizeof(size_t) + sizeof(DataHeader) + MSG_OFFSET, 1MB, 9, 32KB), SocketOptions(0, 0, true), 10, 8, 2, 2, 30.0f, this);
}
Client::~Client()
{
	DestroyClient(clint);
}

TCPClientInterface* Client::GetTCPClient() const
{
	return clint;
}

void MsgHandler(TCPClientInterface& tcpClient, MsgStreamReader streamReader)
{
	Client& clint = *(Client*)tcpClient.GetObj();
	const short type = streamReader.GetType(), msg = streamReader.GetMsg();
	const uint32_t inDataSize = streamReader.GetDataSize();
	char* indata = streamReader.GetData();

	switch (type)
	{
	case TYPE_FILETRANSFER:
		switch (msg)
		{
		case MSG_FILETRANSFER_SEND:
			if (clint.fileReceive.ReadFiles(streamReader))
			{
				_tprintf(_T("Algorithm received; ready to process...\n"));
				tcpClient.SendMsg(TYPE_READY, MSG_READY_PROCESS);
			}
			break;
		case MSG_FILETRANSFER_ABORTED:
			_tprintf(_T("File transfer has been aborted by server\n"));
			break;
		}
		break;
	case TYPE_WORK:
		switch (msg)
		{
		case MSG_WORK_NEW:
			_tprintf(_T("Work load of %d bytes now processing...\n"), inDataSize);

			const uint32_t outSizeMax = Algorithm::GetOutSize(inDataSize);

			auto sndBuff = tcpClient.GetSendBuffer(outSizeMax + MSG_OFFSET);
			*((short*)sndBuff.buffer) = TYPE_WORK;
			*((short*)sndBuff.buffer + 1) = MSG_WORK_COMPLETE;

			const uint32_t dataSize = Algorithm::AlgorithmInOut(indata, inDataSize, sndBuff.buffer + MSG_OFFSET, outSizeMax);
			tcpClient.SendServData(sndBuff, dataSize + MSG_OFFSET);
			break;
		}
		break;
	case TYPE_KICK:
		switch (msg)
		{
		case MSG_KICK_TOOSLOW:
			_tprintf(_T("You have been remove from the server because you took too long to respond/process work request\n"));
			break;
		}
		break;
	}
}

void DisconnectHandler(TCPClientInterface& tcpClient, bool unexpected)
{
	Client& clint = *(Client*)tcpClient.GetObj();
	_tprintf(_T("You have lost connection to server\n"));
}