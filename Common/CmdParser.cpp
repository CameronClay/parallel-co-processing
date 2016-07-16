#include "CmdParser.h"

CmdParser::ParamData::ParamData(TCHAR* param, TCHAR* data)
	:
	param(param),
	data(data)
{}

CmdParser::CmdParser()
	:
	cmd(nullptr)
{}

void CmdParser::ParseCmd(TCHAR* str)
{
	cmd = nullptr;
	args.clear();

	TCHAR* c = (TCHAR*)str;
	for (; *c != _T('\0'); ++c)
	{
		if (*c == _T('-'))
		{
			//if not storing params
			if (!cmd)
			{
				c[-1] = _T('\0');
				cmd = str;
			}
			//if storing params
			else if (c - 3 != args.back().param)
			{
				c[-1] = _T('\0');
				args.back().data = c - 2;
			}
			c[2] = _T('\0');
			args.emplace_back(c);
			++c;
		}
	}

	if (args.empty())
	{
		//store command if no parameters present
		cmd = str;
	}
	else if (c > args.back().param)
	{
		//store last argument data if applicable
		args.back().data = args.back().param + 3;
	}
}

TCHAR* CmdParser::GetCmd() const
{
	return cmd;
}
const std::vector<CmdParser::ParamData>& CmdParser::GetArgs() const
{
	return args;
}
size_t CmdParser::ParamCount() const
{
	return args.size();
}
bool CmdParser::IsSet(const TCHAR* argv) const
{
	for (auto& it : args)
	{
		if (_tcscmp(it.param, argv) == 0)
			return true;
	}
}
bool CmdParser::CmdComp(const TCHAR* str) const
{
	return _tcscmp(cmd, str) == 0;
}