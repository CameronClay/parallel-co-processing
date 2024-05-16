#include <Includes.h>
#include <TCPClientInterface.h>
#include <CmdParser.h>
#include <CmdHandler.h>
#include "ClientCmdParser.h"
#include "Client.h"
#include <GlobalNames.h>

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
#define ARCH "x86"
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
	TCHAR temp[MAX_PATH] = {};
	TCHAR buffer[512] = {};
	FileMisc::GetFullFilePathName(ALGORITHMPATH, temp);
	SetDllDirectory(temp);

	InitializeNetworking();

	assert(SetConsoleCtrlHandler(ConsoleHandler, TRUE));
	atexit(OnExit);

	uint32_t nClients = 0;
	_tprintf(_T("Input number of clients you would like to work with: "));
	_tscanf(_T("%u"), &nClients);
	_getts_s(buffer, 512);

	// 64KB buffer is a guess
	Client client{ nClients, 64KB };

	CmdHandler cmdHandler;
	cmdHandler.AddCmd<ConnectParser>(_T("connect"), std::ref(client));
	cmdHandler.AddCmd<RecvServDataParser>(_T("recvdata"), std::ref(client));
	cmdHandler.AddCmd<DisconnectParser>(_T("disconnect"), std::ref(client));

	do
	{
		_tprintf(_T("Client> "));
		_getts_s(buffer, 512);
	} while (cmdHandler.Process(buffer));

	CleanupNetworking();
	assert(SetConsoleCtrlHandler(ConsoleHandler, FALSE));
	SetDllDirectory(NULL);
	return 0;
}