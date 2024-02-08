#pragma once
#include "CNLIB/Typedefs.h"
#include <tchar.h>
#include <vector>
#include <memory>
#include <regex>

class CmdParser
{
public:
	struct ParamData
	{
		ParamData(const std::tstring& param = {}, const std::tstring& data = {});
		std::tstring param;
		std::tstring data;
	};

	CmdParser();

	bool ParseCmd(TCHAR* str);

	const TCHAR* GetCmd() const;
	const std::vector<ParamData> GetArgs() const;
	size_t ParamCount() const;
	bool IsSet(const TCHAR* argv) const;
	bool CmdComp(const TCHAR* str) const;
private:
	TCHAR* cmd;
	std::vector<ParamData> args;
	static std::wregex RE_CMD;
};