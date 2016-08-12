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

#ifdef _DEBUG
#define CONF "Debug"
#else
#define CONF "Release"
#endif
#if defined _X86_
#define ARCH "x32"
#elif defined _AMD64_
#define ARCH "x64"
#endif
#pragma comment(lib, ARCH"/" CONF"/TCPCS") 

#pragma comment(lib, "DataInterp")

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

void Do(int& a)
{
	a += 5;
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