#pragma once
#include <Includes.h>
#include <MsgStream.h>
#include <File.h>
#include "Event.h"
#include <vector>
#include <thread>

class FileReceive
{
public:
	FileReceive();
	~FileReceive();

	//Returns true on completion
	bool ReadFiles(MsgStreamReader& streamReader);
private:
	bool AdvanceFile();
	void OnCompletion();

	std::vector<FileMisc::FileData> fileList;
	std::tstring dir;
	std::vector<FileMisc::FileData>::iterator curFile;
	File file;
};

