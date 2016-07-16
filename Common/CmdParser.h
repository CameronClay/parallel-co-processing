#pragma once
#include <tchar.h>
#include <vector>
#include <memory>

class CmdParser
{
public:
	struct ParamData
	{
		ParamData(TCHAR* param = nullptr, TCHAR* data = nullptr);
		TCHAR* param;
		TCHAR* data;
	};

	CmdParser();

	void ParseCmd(TCHAR* str);

	TCHAR* GetCmd() const;
	const std::vector<ParamData>& GetArgs() const;
	size_t ParamCount() const;
	bool IsSet(const TCHAR* argv) const;
	bool CmdComp(const TCHAR* str) const;
private:
	TCHAR* cmd;
	std::vector<ParamData> args;
};