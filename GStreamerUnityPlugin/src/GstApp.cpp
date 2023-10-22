#include "GstApp.h"

std::unique_ptr<GstApp> GstApp::_gstApp = nullptr;

GstApp::GstApp(IUnityInterfaces* unity) : _unity(unity) {
	Debug::Log("Initializing GStreamer App");
	gst_init(nullptr, nullptr);
}

void GstApp::Shutdown()
{
	// Never call gst_deinit in the unity dll.
	// You can't start it back after.
	Debug::Log("Disposing GStreamer App");
	_d3d11_device = nullptr;
}

WRL::ComPtr<ID3D11Device> GstApp::getD3D11Device() const
{
	return _unity->Get<IUnityGraphicsD3D11>()->GetDevice();
}

IUnityGraphics* GstApp::getGraphics() const
{
	return _unity->Get<IUnityGraphics>();
}

