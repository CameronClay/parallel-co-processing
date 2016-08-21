#include <Includes.h>
#include <TCPServInterface.h>
#include <CmdParser.h>
#include <CmdHandler.h>
#include <GlobalNames.h>
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
#pragma comment(lib, "Algorithm")

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
	//FileMisc::GetCurDirectory(temp);
	//_tcscat(temp, _T("\\Algorithms"));
	TCHAR temp[MAX_PATH] = {};
	FileMisc::GetFullFilePathName(ALGORITHMPATH, temp);
	SetDllDirectory(temp);

	InitializeNetworking();

	assert(SetConsoleCtrlHandler(ConsoleHandler, TRUE));
	atexit(OnExit);

	Server server{ 2, 64KB };

	TCHAR buffer[512] = {};
	CmdHandler cmdHandler;
	cmdHandler.AddCmd<AllowConnParser>(_T("allowcon"), server.GetTCPServ());
	cmdHandler.AddCmd<ShutdownParser>(_T("shutdown"), server.GetTCPServ());

	do
	{
		_tprintf(_T("Server> "));
		_getts_s(buffer, 512);
	} while (cmdHandler.Process(buffer));

	server.Shutdown();
	CleanupNetworking();
	assert(SetConsoleCtrlHandler(ConsoleHandler, FALSE));

	SetDllDirectory(NULL);
	return 0;
}