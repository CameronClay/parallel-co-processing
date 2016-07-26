#include "WaitableTimer.h"

WaitableTimer::WaitableTimer(bool autoReset)
	:
	timer(CreateWaitableTimer(NULL, !autoReset, NULL))
{}

WaitableTimer::~WaitableTimer()
{
	if (timer != INVALID_HANDLE_VALUE)
	{
		Cancel();
		CloseHandle(timer);
		timer = INVALID_HANDLE_VALUE;
	}
}

bool WaitableTimer::Set(DWORD timeMilli)
{
	LARGE_INTEGER LI = { 0, 0 };

	LI.QuadPart = (LONGLONG)((LONG)timeMilli * -10000);

	return SetWaitableTimer(timer, &LI, (LONG)timeMilli, NULL, NULL, FALSE);
}

bool WaitableTimer::Cancel()
{
	return CancelWaitableTimer(timer);
}

HANDLE WaitableTimer::GetHandle() const
{
	return timer;
}

bool WaitableTimer::Wait(DWORD timeOut)
{
	return WaitForSingleObject(timer, timeOut) == WAIT_OBJECT_0;
}
