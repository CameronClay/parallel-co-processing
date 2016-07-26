#pragma once
#include <Windows.h>

class WaitableTimer
{
public:
	WaitableTimer(bool autoReset);
	~WaitableTimer();

	bool Set(DWORD timeMilli);
	bool Cancel();

	HANDLE GetHandle() const;
	bool Wait(DWORD timeOut = INFINITE);

private:
	HANDLE timer;
};