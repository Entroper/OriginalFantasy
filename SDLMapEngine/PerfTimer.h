#ifndef PERF_TIMER_H
#define PERF_TIMER_H

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>

class PerfTimer
{
public:
	PerfTimer();

	void Start();
	void Stop();
	void Reset();

	LARGE_INTEGER GetDurationTicks();
	LARGE_INTEGER GetTickFrequency();
	double		  GetDurationSeconds();

private:
	LARGE_INTEGER m_StartTime;
	LARGE_INTEGER m_FinishTime;
	LARGE_INTEGER m_TicksElapsed;
	LARGE_INTEGER m_Frequency;
	bool		  m_IsRunning;
};

#endif