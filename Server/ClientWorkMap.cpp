#include "ClientWorkMap.h"

ClientWorkMap::ClientWorkMap()
{}

void ClientWorkMap::Add(ClientData* clint, DWORD64 cursorPos, uint32_t dataSize, uint32_t dataIndex)
{
	std::lock_guard<std::mutex> lock{ mutex };
	workMap.emplace(std::piecewise_construct, std::forward_as_tuple(clint), std::forward_as_tuple(cursorPos, dataSize, dataIndex));
}	

void ClientWorkMap::Change(ClientData* clint, const WorkInfo& wi)
{
	std::lock_guard<std::mutex> lock{ mutex };
	auto it = workMap.find(clint);
	if (it != workMap.end())
		it->second = wi;
	else
		workMap.emplace(clint, wi);
}

bool ClientWorkMap::Remove(ClientData* clint)
{
	std::lock_guard<std::mutex> lock{ mutex };
	return workMap.erase(clint);
}

bool ClientWorkMap::GetClientWork(ClientData* clint, WorkInfo& work)
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