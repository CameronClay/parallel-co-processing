#include "ParamParser.h"

ParamParser::Parameter::Parameter(const TCHAR* param, bool required)
	:
	param(param),
	required(required),
	found(false)
{}

ParamParser::ParamParser(std::initializer_list<Parameter> usableParams, Function<void> execute)
	:
	usableParams(usableParams),
	execute(execute)
{}

void ParamParser::Parse(const CmdParser& cmdParser)
{
	DefaultParams();

	for (const auto& arg : cmdParser.GetArgs())
	{
		if (_tcscmp(arg.param, _T("h")) == 0 || _tcscmp(arg.param, _T("?")) == 0)
		{
			Usage();
			return;
		}

		bool found = false;
		for (auto& usParam : usableParams)
		{
			if (_tcscmp(arg.param, usParam.param) == 0)
			{
				if (!ProcessParam(arg))
				{
					for (auto& usParam : usableParams)
						usParam.found = false;

					_tprintf(_T("Error: Invalid parameter data: %s\n"), arg.data);

					return;
				}

				usParam.found = true;
				found = true;
				break;
			}
		}
		if (!found)
		{
			_tprintf(_T("Error: Invalid parameter: %s\n"), arg.param);
			_tprintf(_T("Use -h or -? for usage details\n"));
			return;
		}
	}
	bool paramFound = true;
	for (auto& usParam : usableParams)
	{
		if (usParam.required && !usParam.found)
		{
			paramFound = false;
			_tprintf(_T("Error: parameter %s required\n"), usParam.param);
		}

		usParam.found = false;
	}

	if (paramFound)
		execute();
	else
		_tprintf(_T("Use -h or -? for usage details\n"));
}