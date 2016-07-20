#pragma once
#include <Includes.h>
#include <CmdParser.h>
#include <TCPServInterface.h>

class AllowConnParser : public ParamParser
{
public:
	template<typename... ExecuteArgs>
	AllowConnParser(ExecuteArgs&&... args)
		:
		ParamParser({ { _T("p"), true }, { _T("ipv"), false }, { _T("nt"), false }, { _T("nct"), false }, { _T("nca"), false } }, { this, &AllowConnParser::Execute, std::forward<ExecuteArgs>(args)... })
	{}
private:
	void DefaultParams()
	{
		port = nullptr;
		ipv = ipv4;
		nThreads = 8;
		nConcThreads = 4;
		nConcAccepts = 2;
	}

	bool ProcessParam(const CmdParser::ParamData& param)
	{
		if (_tcscmp(param.param, _T("p")) == 0)
		{
			port = param.data;
		}
		if (_tcscmp(param.param, _T("ipv")) == 0)
		{
			if (_tcscmp(param.data, _T("ipv4")) == 0)
				ipv = ipv4;
			else if (_tcscmp(param.data, _T("ipv4")) == 0)
				ipv = ipv6;
			else if (_tcscmp(param.data, _T("ipboth")) == 0)
				ipv = ipboth;
			else
				return false;
		}
		else if (_tcscmp(param.param, _T("nt")) == 0)
		{
			nThreads = std::stoull(param.data);
		}
		else if (_tcscmp(param.param, _T("nct")) == 0)
		{
			nConcThreads = std::stoull(param.data);
		}
		else if (_tcscmp(param.param, _T("nca")) == 0)
		{
			nConcAccepts = std::stoull(param.data);
		}
		return true;
	}
	void Usage()
	{
		_tprintf(_T("Usage: -p=Port [-ipv=ipv4/ipv6/ipboth[ipv4] -nt=nThreads[8] -nct=nConcurrentThreads[4] -nca=nConcurrentAccepts[4]]\n"));
	}
	void Execute(TCPServInterface* serv)
	{
		const bool res = serv->AllowConnections(port, CanConnect, nThreads, nConcThreads, ipv, nConcAccepts);
		if (res)
			_tprintf(_T("Now waiting for connections on port %s\n"), port);
		else
			_tprintf(_T("Failed to allow connections on port %s\n"), port);
	}
	static bool CanConnect(TCPServInterface& serv, const Socket& socket)
	{
		return true;
	}

	wchar_t* port;
	IPv ipv;
	DWORD nThreads, nConcThreads, nConcAccepts;
};

class ShutdownParser : public ParamParser
{
public:
	template<typename... ExecuteArgs>
	ShutdownParser(ExecuteArgs&&... args)
		:
		ParamParser({}, { this, &ShutdownParser::Execute, std::forward<ExecuteArgs>(args)... })
	{}
private:
	void DefaultParams() {}
	bool ProcessParam(const CmdParser::ParamData& param)
	{
		return true;
	}
	void Usage()
	{
		_tprintf(_T("Usage: \n"));
	}
	void Execute(TCPServInterface* serv)
	{
		serv->Shutdown();
	}
};