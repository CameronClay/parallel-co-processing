#pragma once
#include <Includes.h>
#include <TCPClientInterface.h>
#include <TCPServInterface.h>
#include <MsgStream.h>
#include <File.h>
#include "Event.h"
#include <vector>
#include <thread>

class FileSend
{
public:
	FileSend(TCPServInterface& serv);
	~FileSend();

	void Initialize();
	void SetFileList(const std::tstring& dir, const std::vector<FileMisc::FileData>& fileList);

	void SendFiles(ClientData* clint);
	void Wait();
	void SendFilesAndWait(ClientData* clint);
	void StopSend();
private:
	enum ThreadState
	{
		RUNNING,
		ABORTING,
		EXITING
	};
	void SendThread();

	TCPServInterface& serv;
	ClientData* clint;
	std::tstring dir;
	std::vector<FileMisc::FileData> fileList;
	EventAutoReset startEv, finishedEv;
	ThreadState threadState;
	std::thread thread;
};

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

