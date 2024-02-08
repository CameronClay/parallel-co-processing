#pragma once
#include <tchar.h>
#include <map>
#include <functional>
#include <CmdParser.h>
#include <CmdHandler.h>
#include "ParamParser.h"

class CmdHandler
{
public:
	CmdHandler() {};
	template<class Struct, typename... Args> 
	void AddCmd(const TCHAR* name, Args&&... args)
	{
		cmds.emplace(name, new Struct(std::forward<Args>(args)...));
	}
	bool Process(TCHAR* str)
	{
		if (!cmdParser.ParseCmd(str))
			_tprintf(_T("Error parsing cmd see usage for more info\n"));

		if (cmdParser.CmdComp(_T("exit")))
			return false;
		else if (cmdParser.CmdComp(_T("help")))
		{
			_tprintf(_T("Commands: help, exit"));

			const size_t size = cmds.size();
			size_t i = 0;
			for (const auto& it : cmds)
			{
				_tprintf(_T(", %s"), it.first);
				if(i == size - 1)
					_tprintf(_T("\n"));
				++i;
			}
		}
		else
		{
			const auto it = cmds.find(cmdParser.GetCmd());
			if (it != cmds.end())
				it->second->Parse(cmdParser);
			else
				_tprintf(_T("Error: invalid cmd, see help for more info\n"));
		}
		return true;
	}
private:
	struct StrCompare
	{
		bool operator()(const TCHAR* lhs, const TCHAR* rhs) const noexcept
		{
			return _tcscmp(lhs, rhs) < 0;
		}
	};

	std::map<const TCHAR*, std::unique_ptr<ParamParser>, StrCompare> cmds;
	CmdParser cmdParser;
};
