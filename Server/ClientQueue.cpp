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
	clint->obj = timePool.construct<TimePoint>(Clock::now());
	otherClients.push(clint);
}

void ClientQueue::RemoveClient(ClientData* clint)
{
	timePool.destruct<TimePoint>((TimePoint*&)clint->obj);
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
	do
	{
		if (!fastClients.pop(clint))
			otherClients.pop(clint);
	} while (clint && !clint->pc.IsConnected());

	return clint;
}