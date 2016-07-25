#pragma once

#include <Includes.h>
#include <TCPServInterface.h>
#include <MemPool.h>
#include <boost/lockfree/queue.hpp>
#include <chrono>

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::time_point<Clock> TimePoint;

class ClientQueue
{
public:
	ClientQueue(uint32_t maxClients);
	~ClientQueue();

	void AddClient(ClientData* clint);
	void RemoveClient(ClientData* clint);
	void EvaluateClient(ClientData* clint, float time);
private:
	ClientData* FindClientForWork();

	boost::lockfree::queue<ClientData*, boost::lockfree::fixed_sized<true>> fastClients, otherClients;
	float acceptedTime;

	//no sync needed because ConnectHandler and DisconnectHandler are synced
	MemPool<> timePool;
};