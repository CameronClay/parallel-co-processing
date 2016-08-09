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

bool CmdParser::ParseCmd(TCHAR* str)
{
	cmd = nullptr;
	args.clear();

	for (TCHAR* c = (TCHAR*)str; *c != _T('\0'); ++c)
	{
		if (cmd && *c == _T('-'))
		{
			//cutoff '-'
			args.emplace_back(c + 1);
			++c;
		}
		else
		{
			if (cmd)
			{
				if (*c == _T(' '))
				{
					*c = '\0';
				}
				else if (*c == _T('='))
				{
					*c = '\0';
					if (args.empty())
						return false;

					args.back().data = c + 1;
				}
			}
			else if(*c == _T(' '))
			{
				*c = _T('\0');
				cmd = str;
			}
		}
	}

	if (args.empty())
	{
		//store command if no parameters present
		cmd = str;
	}

	return true;
}

const TCHAR* CmdParser::GetCmd() const
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
	return false;
}
bool CmdParser::CmdComp(const TCHAR* str) const
{
	return _tcscmp(cmd, str) == 0;
}