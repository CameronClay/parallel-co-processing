#include <Includes.h>
#include <TCPClientInterface.h>
#include <CmdParser.h>
#include <CmdHandler.h>
#include <FileTransfer.h>
#include "ClientCmdParser.h"
#include "Client.h"

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

#pragma comment(lib, "Algorithm")

BOOL CALLBACK ConsoleHandler(DWORD ctrlType);
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
		CleanupNetworking();
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

	Client client{ 64KB };

	TCHAR buffer[512] = {};
	CmdHandler cmdHandler;
	cmdHandler.AddCmd<ConnectParser>(_T("connect"), client.GetTCPClient());
	cmdHandler.AddCmd<RecvServDataParser>(_T("recvdata"), client.GetTCPClient());
	cmdHandler.AddCmd<DisconnectParser>(_T("disconnect"), client.GetTCPClient());

	do
	{
		_tprintf(_T("Client> "));
		_getts_s(buffer, 512);
	} while (cmdHandler.Process(buffer));

	CleanupNetworking();
	assert(SetConsoleCtrlHandler(ConsoleHandler, FALSE));
	return 0;
}