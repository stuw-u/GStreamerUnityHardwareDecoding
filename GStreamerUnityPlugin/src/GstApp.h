#pragma once
#include <gst/gst.h>
#include "Utils.h"
#include <wrl/client.h> // for ComPtr
namespace WRL = Microsoft::WRL; // for ComPtr
#include "../includes/IUnityGraphics.h"
#include "../includes/IUnityGraphicsD3D11.h"
#include <memory>

// A singleton wrapper to the UnityInterface
class GstApp {
protected:
	WRL::ComPtr<ID3D11Device> _d3d11_device;
	IUnityInterfaces* _unity;

public:

	GstApp(IUnityInterfaces* unity);
	void Shutdown();
	WRL::ComPtr<ID3D11Device> getD3D11Device() const;
	IUnityGraphics* getGraphics() const;

	static std::unique_ptr<GstApp> _gstApp;
};