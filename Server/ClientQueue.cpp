#include "ClientQueue.h"

ClientQueue::ClientQueue(uint32_t maxClients)
	:
	fastClients(maxClients),
	otherClients(maxClients),
	timePool(sizeof(TimePoint), maxClients)
{
	//load acceptedTime from dll
}

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

void ClientQueue::EvaluateClient(ClientData* clint, float time)
{
	/*if (time <= ...)
	{
	serv.fastClients.push(clint);
	}
	else
	{
	serv.otherClients.push(clint);
	}*/
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