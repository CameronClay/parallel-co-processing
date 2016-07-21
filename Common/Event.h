#pragma once
#include <mutex>
#include <condition_variable>
#include <atomic>

class Event
{
public:
	Event(bool initialState = false);
	void Set();
	void Reset();
	void Wait();
	bool WaitFor(uint32_t timeMilli);
private:
	std::mutex mutex;
	std::condition_variable cv;
	bool isSet;
};


class EventAutoReset
{
public:
	EventAutoReset(bool initialState = false);
	void Set(uint32_t waitCount = 1);
	void Reset();
	void Wait();
	bool WaitFor(uint32_t timeMilli);
private:
	std::mutex mutex;
	std::condition_variable cv;
	uint32_t counter;
	bool isSet;
};

