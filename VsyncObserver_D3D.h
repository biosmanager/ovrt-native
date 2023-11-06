#pragma once

#include <atomic>
#include <chrono>
#include <thread>

#include <dxgi.h>
#include "Windows.h"

class VsyncObserver_D3D {
public:
	VsyncObserver_D3D(IDXGISwapChain* swapChain);
	~VsyncObserver_D3D();

	bool GetTimeSinceLastVsync(double* secondsSinceLastVsync, uint64_t* frameCounter);
	double GetLastVsyncTimestamp();

private:
	IDXGISwapChain* m_swapChain;
	std::thread m_thread;
	std::atomic_bool m_doStopThread = false;

	std::atomic_llong m_lastVsyncTimestamp = 0;
	std::atomic_uint64_t m_frameCounter = 0;

	double m_performanceCounterFrequency;
};