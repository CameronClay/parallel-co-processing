#include <Includes.h>
#include <TCPServInterface.h>
#include <CmdParser.h>
#include <CmdHandler.h>
#include "ServerCmdParser.h"

#include <stdio.h>
#include <assert.h>
#include <tchar.h>
#include <Windows.h>

#ifdef _X86_
#pragma comment(lib, "x32/TCPCS")
#else
#pragma comment(lib, "x64/TCPCS")
#endif

static const WCHAR PORT[] = L"985";

TCPServInterface* serv = nullptr;

BOOL CALLBACK ConsoleHandler(DWORD ctrlType);
int _tmain(int argc, TCHAR** argv);

void MsgHandler(TCPServInterface& serv, ClientData* const clint, MsgStreamReader streamReader);
void DisconnectHandler(TCPServInterface& serv, ClientData* data, bool unexpected);
void ConnectHandler(TCPServInterface& serv, ClientData* data);

BOOL CALLBACK ConsoleHandler(DWORD ctrlType)
{
	switch (ctrlType)
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
		break;
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		DestroyServer(serv);
		CleanupNetworking();
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

void MsgHandler(TCPServInterface& serv, ClientData* const clint, MsgStreamReader streamReader)
{

}

void DisconnectHandler(TCPServInterface& serv, ClientData* data, bool unexpected)
{

}

void ConnectHandler(TCPServInterface& serv, ClientData* data)
{

}

int _tmain(int argc, TCHAR** argv)
{
	InitializeNetworking();

	TCHAR buffer[512] = {};
	CmdHandler cmdHandler;
	cmdHandler.AddCmd<AllowConnParser>(_T("allowcon"), std::ref(serv));
	cmdHandler.AddCmd<ShutdownParser>(_T("shutdown"), std::ref(serv));

	assert(SetConsoleCtrlHandler(ConsoleHandler, TRUE));
	serv = CreateServer(MsgHandler, ConnectHandler, DisconnectHandler);

	do
	{
		_tprintf(_T("Server> "));
		_getts_s(buffer, 512);
	} while (cmdHandler.Process(buffer));

	DestroyServer(serv);
	CleanupNetworking();
	assert(SetConsoleCtrlHandler(ConsoleHandler, FALSE));
	return 0;
}