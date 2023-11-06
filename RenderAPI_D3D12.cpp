#include "RenderAPI.h"
#include "PlatformBase.h"
#include "VsyncObserver_D3D.h"

#include <cmath>

// Direct3D 12 implementation of RenderAPI.


#if SUPPORT_D3D12

#include <assert.h>
#include <d3d12.h>
#include "Unity/IUnityGraphicsD3D12.h"


class RenderAPI_D3D12 : public RenderAPI
{
public:
	RenderAPI_D3D12();
	virtual ~RenderAPI_D3D12() { }

	virtual void ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces);
	virtual bool GetTimeSinceLastVsync(double* secondsSinceLastVsync, uint64_t* frameCounter);
	virtual double GetLastVsyncTimestamp();

private:
	void CreateResources();
	void ReleaseResources();

private:
	ID3D12Device* m_Device;
	VsyncObserver_D3D* m_vsyncObserver;
};


RenderAPI* CreateRenderAPI_D3D12()
{
	return new RenderAPI_D3D12();
}


RenderAPI_D3D12::RenderAPI_D3D12()
	: m_Device(NULL),
	m_vsyncObserver(NULL)
{
}


void RenderAPI_D3D12::CreateResources()
{

}


void RenderAPI_D3D12::ReleaseResources()
{
	delete m_vsyncObserver;
	// use SAFE_RELEASE
}


void RenderAPI_D3D12::ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces)
{
	switch (type)
	{
	case kUnityGfxDeviceEventInitialize:
	{
		IUnityGraphicsD3D12v7* d3d = interfaces->Get<IUnityGraphicsD3D12v7>();
		m_Device = d3d->GetDevice();
		m_vsyncObserver = new VsyncObserver_D3D(d3d->GetSwapChain());
		CreateResources();
		break;
	}
	case kUnityGfxDeviceEventShutdown:
		ReleaseResources();
		break;
	}
}

bool RenderAPI_D3D12::GetTimeSinceLastVsync(double* secondsSinceLastVsync, uint64_t* frameCounter) {
	if (!m_vsyncObserver) return false;

	return m_vsyncObserver->GetTimeSinceLastVsync(secondsSinceLastVsync, frameCounter);
}

double RenderAPI_D3D12::GetLastVsyncTimestamp()
{
	if (!m_vsyncObserver) return -1;

	return m_vsyncObserver->GetLastVsyncTimestamp();
}


#endif // #if SUPPORT_D3D12
