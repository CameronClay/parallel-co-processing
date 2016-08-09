#pragma once
#include <Includes.h>
#include <ParamParser.h>
#include <TCPClientInterface.h>
#include <MessagesExt.h>

class ConnectParser : public ParamParser
{
public:
	template<typename... ExecuteArgs>
	ConnectParser(ExecuteArgs&&... args)
		:
		ParamParser({ { _T("ip"), true },{ _T("p"), true },{ _T("ipv6"), false },{ _T("t"), false } }, { this, &ConnectParser::Execute, std::forward<ExecuteArgs>(args)... })
	{}
private:
	void DefaultParams()
	{
		ip = port = nullptr;
		ipv6 = false;
		timeout = 5.0f;
	}
	bool ProcessParam(const CmdParser::ParamData& param)
	{
		if (_tcscmp(param.param, _T("ip")) == 0)
			ip = param.data;
		else if (_tcscmp(param.param, _T("p")) == 0)
			port = param.data;
		else if (_tcscmp(param.param, _T("ipv6")) == 0)
		{
			if (_tcscmp(param.data, _T("true")) == 0)
				ipv6 = true;
			else if (_tcscmp(param.data, _T("false")) == 0)
				ipv6 = false;
			else
				return false;
		}
		else if (_tcscmp(param.param, _T("t")) == 0)
			timeout = std::stof(param.data);

		return true;
	}
	void Usage()
	{
		_tprintf(_T("Usage: -ip=IP address -p=port -ipv6=true/false[false] [-t=timesec[5.0]]\n"));
	}
	void Execute(TCPClientInterface* clint)
	{
		const bool res = clint->Connect(ip, port, ipv6, timeout);
		if (res)
			_tprintf(_T("Successfully connected to %s on port %s\n"), ip, port);
		else
			_tprintf(_T("Failed to connect to %s on port %s\n"), ip, port);
	}

	wchar_t *ip, *port;
	bool ipv6;
	float timeout;
};

class RecvServDataParser : public ParamParser
{
public:
	template<typename... ExecuteArgs>
	RecvServDataParser(ExecuteArgs&&... args)
		:
		ParamParser({ { _T("nt"), false },{ _T("nct"), false } }, { this, &RecvServDataParser::Execute, std::forward<ExecuteArgs>(args)... })
	{}
private:
	void DefaultParams()
	{
		nThreads = 2;
		nConcThreads = 1;
	}
	bool ProcessParam(const CmdParser::ParamData& param)
	{
		if (_tcscmp(param.param, _T("nt")) == 0)
		{
			nThreads = std::stoull(param.data);
		}
		if (_tcscmp(param.param, _T("nct")) == 0)
		{
			nConcThreads = std::stoull(param.data);
		}

		return true;
	}
	void Usage()
	{
		_tprintf(_T("Usage: [-nt=nThreads[2] -nct=nConcThreads[1]]\n"));
	}
	void Execute(TCPClientInterface* clint)
	{
		const bool res = clint->RecvServData(nThreads, nConcThreads);
		if (res)
			_tprintf(_T("Now receiving data from serv\n"));
		else
			_tprintf(_T("Failed to receive data from serv\n"));

		if (res)
			clint->SendMsg(TYPE_READY, MSG_READY_INITIALIZED);
	}

	DWORD nThreads, nConcThreads;
};

class DisconnectParser : public ParamParser
{
public:
	template<typename... ExecuteArgs>
	DisconnectParser(ExecuteArgs&&... args)
		:
		ParamParser({}, { this, &DisconnectParser::Execute, std::forward<ExecuteArgs>(args)... })
	{}
private:
	void DefaultParams(){}
	bool ProcessParam(const CmdParser::ParamData& param)
	{
		return true;
	}
	void Usage()
	{
		_tprintf(_T("Usage: \n"));
	}
	void Execute(TCPClientInterface* clint)
	{
		clint->Disconnect();
	}
};