#include <Includes.h>
#include <TCPServInterface.h>
#include <CmdParser.h>
#include <CmdHandler.h>
#include <FileTransfer.h>
#include "ServerCmdParser.h"
#include "Server.h"

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

BOOL CALLBACK ConsoleHandler(DWORD ctrlType);
void OnExit();
int _tmain(int argc, TCHAR** argv);

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
		//DestroyServer(serv);
		//CleanupNetworking();
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

void OnExit()
{
	CleanupNetworking();
}

int _tmain(int argc, TCHAR** argv)
{
	InitializeNetworking();

	assert(SetConsoleCtrlHandler(ConsoleHandler, TRUE));
	atexit(OnExit);

	Server server;

	TCHAR buffer[512] = {};
	CmdHandler cmdHandler;
	cmdHandler.AddCmd<AllowConnParser>(_T("allowcon"), server.GetTCPServ());
	cmdHandler.AddCmd<ShutdownParser>(_T("shutdown"), server.GetTCPServ());

	do
	{
		_tprintf(_T("Server> "));
		_getts_s(buffer, 512);
	} while (cmdHandler.Process(buffer));

	CleanupNetworking();
	assert(SetConsoleCtrlHandler(ConsoleHandler, FALSE));
	return 0;
}