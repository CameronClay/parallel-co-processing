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

	void SendFiles(ClientData* clint, const std::vector<FileMisc::FileData>& fileList);
	void Wait();
	void SendFilesAndWait(ClientData* clint, const std::vector<FileMisc::FileData>& fileList);
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
	void OnCompletion();
private:
	std::vector<FileMisc::FileData> fileList;
	std::vector<FileMisc::FileData>::iterator curFile;
	File file;
};

