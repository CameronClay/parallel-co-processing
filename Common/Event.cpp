#include "Event.h"

Event::Event(bool initialState)
	: 
	isSet(initialState)
{}

void Event::Set()
{
	std::lock_guard<std::mutex> lock{ mutex };
	isSet = true;
	cv.notify_all();
}

void Event::Reset()
{
	std::lock_guard<std::mutex> lock{ mutex };
	isSet = false;
}

void Event::Wait()
{
	std::unique_lock<std::mutex> lock{ mutex };
	if (isSet)
		return;

	cv.wait(lock, [this] { return isSet; });
}

bool Event::WaitFor(uint32_t timeMilli)
{
	std::unique_lock<std::mutex> lock{ mutex };
	if (isSet)
		return true;

	return cv.wait_for(lock, std::chrono::milliseconds(timeMilli), [this] { return isSet; });
}


EventAutoReset::EventAutoReset(bool initialState)
	:
	counter(0),
	isSet(initialState)
{}

void EventAutoReset::Set(uint32_t waitCount)
{
	std::lock_guard<std::mutex> lock{ mutex };
	counter = waitCount;
	isSet = true;
	cv.notify_all();
}

void EventAutoReset::Reset()
{
	std::lock_guard<std::mutex> lock{ mutex };
	counter = 0;
	isSet = false;
}

void EventAutoReset::Wait()
{
	std::unique_lock<std::mutex> lock{ mutex };
	if (isSet)
	{
		if (--counter == 0)
			isSet = false;
		return;
	}

	cv.wait(lock, [this] { return isSet; });

	if (--counter == 0)
		isSet = false;
}

bool EventAutoReset::WaitFor(uint32_t timeMilli)
{
	std::unique_lock<std::mutex> lock{ mutex };
	if (isSet)
	{
		if (--counter == 0)
			isSet = false;
		return true;
	}

	if (cv.wait_for(lock, std::chrono::milliseconds(timeMilli), [this] { return isSet; }))
	{
		if (--counter == 0)
			isSet = false;
		return true;
	}

	return false;
}