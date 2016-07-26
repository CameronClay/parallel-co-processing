#include "ClientWorkMap.h"

ClientWorkMap::Work::Work()
	:
	cursorPos(std::numeric_limits<size_t>::max()),
	dataSize(std::numeric_limits<size_t>::max())
{}

ClientWorkMap::Work::Work(size_t cursorPos, size_t dataSize)
	:
	cursorPos(cursorPos),
	dataSize(dataSize)
{}

ClientWorkMap::ClientWorkMap()
{}

void ClientWorkMap::Add(ClientData* clint, size_t cursorPos, size_t dataSize)
{
	std::lock_guard<std::mutex> lock{ mutex };
	workMap.emplace(std::piecewise_construct, std::forward_as_tuple(clint), std::forward_as_tuple(cursorPos, dataSize));
}	

void ClientWorkMap::Change(ClientData* clint, size_t cursorPos, size_t dataSize)
{
	std::lock_guard<std::mutex> lock{ mutex };
	auto it = workMap.find(clint);
	if (it != workMap.end())
	{
		it->second.cursorPos = cursorPos;
		it->second.dataSize = dataSize;
	}
	else
	{
		workMap.emplace(std::piecewise_construct, std::forward_as_tuple(clint), std::forward_as_tuple(cursorPos, dataSize));
	}
}

bool ClientWorkMap::Remove(ClientData* clint)
{
	std::lock_guard<std::mutex> lock{ mutex };
	return workMap.erase(clint);
}

bool ClientWorkMap::GetClientWork(ClientData* clint, Work& work)
{
	std::lock_guard<std::mutex> lock{ mutex };
	auto it = workMap.find(clint);
	if (it != workMap.end())
	{
		work = it->second;
		workMap.erase(it);
		return true;
	}
	return false;
}