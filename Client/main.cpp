#include "Includes.h"
#include "TCPClientInterface.h"

#include <assert.h>
#include <tchar.h>
#include <Windows.h>

#ifdef _X86_
#pragma comment(lib, "x32/TCPCS")
#else
#pragma comment(lib, "x64/TCPCS")
#endif

TCPClientInterface* client = nullptr;

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
		DestroyClient(client);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

void MsgHandler(TCPClientInterface&, MsgStreamReader streamReader)
{

}

void DisconnectHandler(TCPClientInterface& client, bool unexpected)
{
	
}

int _tmain(int argc, TCHAR** argv)
{
	assert(SetConsoleCtrlHandler(ConsoleHandler, TRUE));
	client = CreateClient(MsgHandler, DisconnectHandler);

	DestroyClient(client);
	assert(SetConsoleCtrlHandler(ConsoleHandler, FALSE));
	return 0;
}