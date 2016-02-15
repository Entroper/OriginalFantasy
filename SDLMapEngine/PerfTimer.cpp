#include "PerfTimer.h"

PerfTimer::PerfTimer()
{
	m_StartTime.QuadPart  = 0;
	m_FinishTime.QuadPart = 0;
	m_TicksElapsed.QuadPart = 0;

	QueryPerformanceFrequency(&m_Frequency);

	m_IsRunning = false;
}

void PerfTimer::Start()
{
	QueryPerformanceCounter(&m_StartTime);
	m_IsRunning = true;
}

void PerfTimer::Stop()
{
	QueryPerformanceCounter(&m_FinishTime);
	m_TicksElapsed.QuadPart = m_TicksElapsed.QuadPart + m_FinishTime.QuadPart - m_StartTime.QuadPart;
	m_IsRunning = false;
}

void PerfTimer::Reset()
{
	// Reset the performance timer to its default value.
	m_StartTime.QuadPart  = 0;
	m_FinishTime.QuadPart = 0;
	m_TicksElapsed.QuadPart = 0;
}

LARGE_INTEGER PerfTimer::GetDurationTicks()
{
	return m_TicksElapsed;
}

LARGE_INTEGER PerfTimer::GetTickFrequency()
{
	return m_Frequency;
}

double PerfTimer::GetDurationSeconds()
{
	double Ret = (double)(GetDurationTicks().QuadPart) / (double)(GetTickFrequency().QuadPart);
	return Ret;
}
