#pragma once

//Fixes prob with min/max
#define NOMINMAX

#include <Includes.h>
#include <TCPServInterface.h>
#include <unordered_map>
#include <mutex>

class ClientWorkMap
{
public:
	struct Work
	{
		Work();
		Work(size_t cursorPos, size_t dataSize);
		size_t cursorPos, dataSize;
	};

	ClientWorkMap();

	void Add(ClientData* clint, size_t cursorPos, size_t dataSize);
	void Change(ClientData* clint, size_t cursorPos, size_t dataSize);
	bool Remove(ClientData* clint);
	bool GetClientWork(ClientData* clint, Work& work);
private:
	std::unordered_map<ClientData*, Work> workMap;
	std::mutex mutex;
};