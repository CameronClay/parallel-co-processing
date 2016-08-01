#include "Client.h"
#include <Algorithm.h>
#include <MessagesExt.h>

Client::Client()
{
	clint = CreateClient(MsgHandler, DisconnectHandler, 5, BufferOptions(), SocketOptions(), 10, 8, 2, 2, 30.0f, this);
}
Client::~Client()
{
	DestroyClient(clint);
}

void MsgHandler(TCPClientInterface& tcpClient, MsgStreamReader streamReader)
{
	Client& clint = *(Client*)tcpClient.GetObj();
	const short type = streamReader.GetType(), msg = streamReader.GetMsg();
	switch (type)
	{
	case TYPE_FILETRANSFER:
		switch (msg)
		{
		case MSG_FILETRANSFER_SEND:
			clint.fileReceive.ReadFiles(streamReader);
			break;
		case MSG_FILETRANSFER_ABORTED:
			//notify user in terminal
			break;
		}
		break;
	case TYPE_WORK:
		switch (msg)
		{
		case MSG_WORK_NEW:
			//notify user of workload?
			uint32_t outSizeMax = Algorithm::GetOutSize(streamReader.GetDataSize());
			auto strm = tcpClient.CreateOutStream(outSizeMax, TYPE_WORK, MSG_WORK_COMPLETE);
			Algorithm::AlgorithmInOut(streamReader.GetData(), streamReader.GetDataSize(), strm.GetData(), outSizeMax);
			tcpClient.SendServData(strm);
			break;
		}
		break;
	case TYPE_KICK:
		switch (msg)
		{
		case MSG_KICK_TOOSLOW:
			//notify user in terminal
			break;
		}
		break;
	}
}

void DisconnectHandler(TCPClientInterface& tcpClient, bool unexpected)
{
	Client& clint = *(Client*)tcpClient.GetObj();
	//notify user of disconnect
}