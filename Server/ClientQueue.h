#pragma once

#include <Includes.h>
#include <TCPServInterface.h>
#include <MemPool.h>
#include <boost/lockfree/queue.hpp>
#include <chrono>
#include "PerformanceModel.h"

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::time_point<Clock> TimePoint;

class ClientQueue
{
public:
	ClientQueue(uint32_t maxClients);
	~ClientQueue();

	void AddClient(ClientData* clint);
	void RemoveClient(ClientData* clint);
	bool EvaluateClient(ClientData* clint, float time);

	ClientData* FindClientForWork();
private:
	boost::lockfree::queue<ClientData*, boost::lockfree::fixed_sized<true>> fastClients, otherClients;
	PerformanceModel perfModel;;

	MemPoolSync<> timePool;
};