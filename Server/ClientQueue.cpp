#include "ClientQueue.h"
#include <DataInterp.h>

ClientQueue::ClientQueue(uint32_t maxClients)
	:
	fastClients(maxClients),
	otherClients(maxClients),
	timePool(sizeof(uint32_t), maxClients)
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
	if (time > DataInterp::MAXTIME)
		return false;

	if (time <= DataInterp::ACCEPTEDTIME)
		fastClients.push(clint);
	else
		otherClients.push(clint);

	return true;
}

ClientData* ClientQueue::FindClientForWork()
{
	ClientData* clint = nullptr;
	bool ret = fastClients.pop(clint); //for some reason occasionally clint is still valid if ret is false
	if (!ret)
		ret = otherClients.pop(clint);

	if (ret)
		return clint;

	return nullptr;
}