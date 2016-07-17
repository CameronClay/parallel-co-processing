#pragma once
#include <Includes.h>
#include <TCPClientInterface.h>
#include <string>
#include "CmdParser.h"
#include "Function.h"

class ParamParser
{
public:
	struct Parameter
	{
		Parameter(const TCHAR* param, bool required);

		const TCHAR* param;
		bool required, found;
	};
	ParamParser(std::initializer_list<Parameter> usableParams, Function<void> execute);

	void Parse(const CmdParser& cmdParser);
private:
	virtual void DefaultParams() = 0;
	virtual bool ProcessParam(const CmdParser::ParamData& param) = 0;
	virtual void Usage() = 0;

	std::vector<Parameter> usableParams;
	Function<void> execute;
};
