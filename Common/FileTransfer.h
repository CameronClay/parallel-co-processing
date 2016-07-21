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
	FileSend();
	~FileSend();

	void SendFiles(TCPServInterface* serv, ClientData* clint, const std::vector<FileMisc::FileData>& fileList);
	void Wait();
	void SendFilesAndWait(TCPServInterface* serv, ClientData* clint, const std::vector<FileMisc::FileData>& fileList);
	void StopSend();
private:
	enum ThreadState
	{
		RUNNING,
		ABORTING,
		EXITING
	};
	friend static void SendThread(FileSend& fileSend);

	TCPServInterface* serv;
	ClientData* clint;
	std::vector<FileMisc::FileData> fileList;
	std::thread thread;
	EventAutoReset startEv, finishedEv;
	ThreadState threadState;
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
	//size_t curFile;
	std::vector<FileMisc::FileData>::iterator curFile;
	File file;
	DWORD totalRead;
};

