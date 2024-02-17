#pragma once
#include "GstApp.h"
#include "Utils.h"
#include <gst/gst.h>
#include <d3d11.h>

// Manages a single UDP DX11 pipeline.
class GstAppPipeline
{
private:
	u32 id;
	const GstApp* app;
	GstElement* pipeline;			// The Gst pipeline ref. Required to dispose of the pipeline.
	ID3D11Texture2D* texture;		// The target texture GStreamer will draw to. Doesn't need to be the same res as the stream.
	HANDLE sharedHandle;			// SharedHandle required for GStreamer to draw on our texture from a different device.
	GThread* pipelineLoopThread;
	u32 textureWidth;
	u32 textureHeight;
	GstBus* bus;
	u32 busWatchId;

public:
	GMainLoop* mainLoop;
	HANDLE GetSharedHandle();
	ID3D11Texture2D* GetTexturePtr();

	GstAppPipeline(u32 id, const GstApp* app, u32 width, u32 height, char* uri);
	~GstAppPipeline();

	bool CreateTexture();
	static void ReleaseTexture(ID3D11Texture2D* texture);
};

