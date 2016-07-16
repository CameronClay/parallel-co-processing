#include <Includes.h>
#include <TCPServInterface.h>
#include <CmdParser.h>

#include <stdio.h>
#include <assert.h>
#include <tchar.h>
#include <Windows.h>

#ifdef _X86_
#pragma comment(lib, "x32/TCPCS")
#else
#pragma comment(lib, "x64/TCPCS")
#endif

TCPServInterface* serv = nullptr;

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
		DestroyServer(serv);
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
	TCHAR buffer[512] = {};
	CmdParser cmdParser;
	assert(SetConsoleCtrlHandler(ConsoleHandler, TRUE));
	serv = CreateServer(MsgHandler, ConnectHandler, DisconnectHandler);
	do
	{
		_getts_s(buffer, 512);
		cmdParser.ParseCmd(buffer);
	} while (!cmdParser.CmdComp(_T("exit")));
	DestroyServer(serv);
	assert(SetConsoleCtrlHandler(ConsoleHandler, FALSE));
	return 0;
}