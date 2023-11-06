#include "VsyncObserver_D3D.h"

#include "Unity/IUnityLog.h"

#include <string>

extern IUnityLog* s_Log;

void LogSwapChainInfo(IDXGISwapChain* swapChain);

VsyncObserver_D3D::VsyncObserver_D3D(IDXGISwapChain* swapChain) : m_swapChain(swapChain)
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	m_performanceCounterFrequency = (double)freq.QuadPart;

	if (!m_swapChain)
	{
		UNITY_LOG_WARNING(s_Log, "[OVRT] Swap chain is null. This is normal when running in editor. Cannot measure accurate VSync timing.");
		return;
	}
	else {
		UNITY_LOG(s_Log, "[OVRT] Initialized VSync observer on swapchain.");
	}

	//LogSwapChainInfo(m_swapChain);

	m_thread = std::thread([&]() {
		IDXGIOutput* output;
		LARGE_INTEGER counter;

		m_swapChain->GetContainingOutput(&output);

		QueryPerformanceCounter(&counter);
		m_lastVsyncTimestamp = counter.QuadPart;
		
		while (!m_doStopThread)
		{
			if (m_swapChain && output)
			{
				output->WaitForVBlank();
				QueryPerformanceCounter(&counter);

				m_lastVsyncTimestamp = counter.QuadPart;
				m_frameCounter++;
			}
			//std::this_thread::yield();
		}
	});
}

VsyncObserver_D3D::~VsyncObserver_D3D()
{
	m_doStopThread = true;
	m_thread.join();
}

bool VsyncObserver_D3D::GetTimeSinceLastVsync(double* secondsSinceLastVsync, uint64_t* frameCounter)
{
	if (!m_swapChain) return false;

	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);

	*secondsSinceLastVsync = (now.QuadPart - m_lastVsyncTimestamp) / m_performanceCounterFrequency;
	*frameCounter = m_frameCounter;
	return true;
}

double VsyncObserver_D3D::GetLastVsyncTimestamp()
{
	if (!m_swapChain) return 0;

	return m_lastVsyncTimestamp / m_performanceCounterFrequency;
}

void LogSwapChainInfo(IDXGISwapChain* swapChain) 
{
	if (!swapChain) return;

	//DXGI_SWAP_CHAIN_DESC swapChainDesc;
	//swapChain->GetDesc(&swapChainDesc);

	IDXGIOutput* output;
	DXGI_OUTPUT_DESC outputDesc;

	swapChain->GetContainingOutput(&output);
	output->GetDesc(&outputDesc);

	std::string coordsStr = "Coords: " +
		std::to_string(outputDesc.DesktopCoordinates.left) + "," +
		std::to_string(outputDesc.DesktopCoordinates.right) + "," +
		std::to_string(outputDesc.DesktopCoordinates.top) + "," +
		std::to_string(outputDesc.DesktopCoordinates.bottom);

	UNITY_LOG(s_Log, coordsStr.c_str());
}
