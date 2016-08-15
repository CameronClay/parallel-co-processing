#pragma once

//Fixes prob with min/max
#define NOMINMAX

#include <Includes.h>
#include <TCPServInterface.h>
#include <unordered_map>
#include <mutex>
#include <DataInterp.h>

class ClientWorkMap
{
public:
	ClientWorkMap();

	void Add(ClientData* clint, DWORD64 cursorPos, uint32_t dataSize, uint32_t dataIndex);
	void Change(ClientData* clint, const WorkInfo& wi);
	bool Remove(ClientData* clint);
	bool GetClientWork(ClientData* clint, WorkInfo& work);
	bool Empty() const;
private:
	std::unordered_map<ClientData*, WorkInfo> workMap;
	std::mutex mutex;
};