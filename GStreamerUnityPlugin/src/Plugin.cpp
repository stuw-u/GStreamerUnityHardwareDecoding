#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#include "Utils.h"
#include "DebugCpp.h"
#include "GstApp.h"
#include "GstAppPipeline.h"
#include "../includes/IUnityInterface.h"
#include "../includes/IUnityGraphics.h"
#include "../includes/IUnityGraphicsD3D11.h"

std::mutex lock_;


UnityExport int GetDebugLog (char* message, int maxLength) {
	return Debug::GetDebugLog(message, maxLength);
}

UnityExport void ClearDebugLog () {
	Debug::ClearDebugLog();
}



UnityExport void* CreatePipeline(u32 id, u32 width, u32 height, char* uri) {
	return new GstAppPipeline(id, GstApp::_gstApp.get(), width, height, uri);
}

UnityExport void DisposePipeline(void* pipelinePtr) {
	GstAppPipeline* pipeline = (GstAppPipeline*)pipelinePtr;
	delete pipeline;
}

UnityExport void* GetPipelineTexturePtr(void* pipelinePtr) {
	std::lock_guard<std::mutex> guard(lock_);
	GstAppPipeline* pipeline = (GstAppPipeline*)pipelinePtr;
	return pipeline->GetTexturePtr();
}

static void UNITY_INTERFACE_API OnRenderEvent(int eventID, void* dataPtr)
{
	if (dataPtr == nullptr) return;

	std::lock_guard<std::mutex> guard(lock_);
	switch (eventID) {
	case EVENT_CREATE_TEXTURE:
		
		((GstAppPipeline*)dataPtr)->CreateTexture();

		break;
	case EVENT_DISPOSE_TEXTURE:

		GstAppPipeline::ReleaseTexture((ID3D11Texture2D*)dataPtr);

		break;
	}
}

// Freely defined function to pass a callback to plugin-specific scripts
UnityExport UnityRenderingEventAndData UNITY_INTERFACE_API GetRenderEventFunc()
{
	return OnRenderEvent;
}

// Unity graphic device event
UnityExport void OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType)
{
	if (eventType == kUnityGfxDeviceEventShutdown) GstApp::_gstApp->Shutdown();
}

// Unity plugin load event
UnityExport void UnityPluginLoad(IUnityInterfaces* interfaces)
{
	Debug::InitiateDebug();
	GstApp::_gstApp = std::make_unique<GstApp>(interfaces);
	GstApp::_gstApp->getGraphics()->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);
}

// Unity plugin unload event
UnityExport void UnityPluginUnload()
{
	GstApp::_gstApp->getGraphics()->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
	GstApp::_gstApp.reset();
	Debug::ReleaseDebug();
}