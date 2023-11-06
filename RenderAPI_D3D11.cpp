#include "RenderAPI.h"
#include "PlatformBase.h"
#include "VsyncObserver_D3D.h"


// Direct3D 11 implementation of RenderAPI.

#if SUPPORT_D3D11

#include <assert.h>
#include <d3d11.h>
#include "Unity/IUnityGraphicsD3D11.h"

class RenderAPI_D3D11 : public RenderAPI
{
public:
	RenderAPI_D3D11();
	virtual ~RenderAPI_D3D11() { }

	virtual void ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces);
	virtual bool GetTimeSinceLastVsync(double* secondsSinceLastVsync, uint64_t* frameCounter);
	virtual double GetLastVsyncTimestamp();

private:
	void CreateResources();
	void ReleaseResources();

private:
	ID3D11Device* m_Device;
	VsyncObserver_D3D* m_vsyncObserver;
};


RenderAPI* CreateRenderAPI_D3D11()
{
	return new RenderAPI_D3D11();
}


RenderAPI_D3D11::RenderAPI_D3D11()
	: m_Device(NULL),
	m_vsyncObserver(NULL)
{
}


void RenderAPI_D3D11::ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces)
{
	switch (type)
	{
	case kUnityGfxDeviceEventInitialize:
	{
		IUnityGraphicsD3D11* d3d = interfaces->Get<IUnityGraphicsD3D11>();
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

bool RenderAPI_D3D11::GetTimeSinceLastVsync(double* secondsSinceLastVsync, uint64_t* frameCounter) {
	if (!m_vsyncObserver) return false;
	
	return m_vsyncObserver->GetTimeSinceLastVsync(secondsSinceLastVsync, frameCounter);
}

double RenderAPI_D3D11::GetLastVsyncTimestamp()
{
	if (!m_vsyncObserver) return -1;

	return m_vsyncObserver->GetLastVsyncTimestamp();
}


void RenderAPI_D3D11::CreateResources()
{

}


void RenderAPI_D3D11::ReleaseResources()
{
	delete m_vsyncObserver;
	// use SAFE_RELEASE
}


#endif // #if SUPPORT_D3D11
