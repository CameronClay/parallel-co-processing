#include "ClientQueue.h"
#include <DataInterp.h>

ClientQueue::ClientQueue(uint32_t maxClients)
	:
	fastClients(maxClients),
	otherClients(maxClients),
	timePool(sizeof(TimePoint), maxClients)
{}

ClientQueue::~ClientQueue()
{}

void ClientQueue::AddClient(ClientData* clint)
{
	if(!clint->obj)
		clint->obj = timePool.construct<TimePoint>();

	otherClients.push(clint);
}

void ClientQueue::RemoveClient(ClientData* clint)
{
	timePool.destruct<TimePoint>((TimePoint*&)(clint->obj));
}

bool ClientQueue::EvaluateClient(ClientData* clint, float time)
{
	auto res = perfModel.EvaluateTime(time);

	if (res == PerformanceModel::TOOSLOW)
		return false;

	if (res == PerformanceModel::FAST)
		fastClients.push(clint);
	else
		otherClients.push(clint);

	return true;
}

ClientData* ClientQueue::FindClientForWork()
{
	ClientData* clint = nullptr;
	bool ret = fastClients.pop(clint);
	if (!ret)
		ret = otherClients.pop(clint);

	if (ret)
		return clint;

	return nullptr;
}