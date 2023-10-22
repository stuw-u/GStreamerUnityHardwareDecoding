#pragma once
#include "GstApp.h"
#include "Utils.h"
#include <gst/gst.h>
#include <d3d11.h>

class GstAppPipeline
{
private:
	u32 id;
	const GstApp* app;
	GstElement* pipeline;
	ID3D11Texture2D* texture;
	HANDLE sharedHandle;
	GThread* pipelineLoopThread;
	u32 textureWidth;
	u32 textureHeight;

public:
	GMainLoop* mainLoop;
	HANDLE GetSharedHandle();
	ID3D11Texture2D* GetTexturePtr();

	GstAppPipeline(u32 id, const GstApp* app, u32 width, u32 height, char* uri);
	~GstAppPipeline();

	bool CreateTexture();
	static void ReleaseTexture(ID3D11Texture2D* texture);
};

