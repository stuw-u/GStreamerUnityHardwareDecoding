#include "GstAppPipeline.h"


#include <string>
#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <gst/video/video.h>
#include <d3d11.h>
#include <d3d11sdklayers.h>
#include <dxgi1_2.h>
#include <dxgiformat.h>

#include <d3d11.h>
#include <d3d11_1.h>
#include <glib-object.h>
#include <glib.h>


// No idea how gstreamer runs if we don't explicitely tell
// gstreamer to use this given loop??
gpointer GstMainLoopFunction (gpointer data)
{
	Debug::Log("Entering main loop");
	GstAppPipeline* pipeline = static_cast<GstAppPipeline*>(data);
	pipeline->mainLoop = g_main_loop_new(nullptr, FALSE);
	g_main_loop_run(pipeline->mainLoop);
	Debug::Log("Quitting main loop");

	return NULL;
}


// Callback from GStreamer when it's ready to draw on our shared ID3D11Texture.
static GstFlowReturn OnBeginDraw (GstElement* videosink, gpointer data)
{
	GstAppPipeline* pipeline = static_cast<GstAppPipeline*>(data);
	HANDLE sharedHandle = pipeline->GetSharedHandle();
	if (sharedHandle == nullptr) {
		return GST_FLOW_OK;
	}

	// GStreamer draws on our texture (assumed same size)
	g_signal_emit_by_name(videosink, "draw", (gpointer)sharedHandle, D3D11_RESOURCE_MISC_SHARED, 0, 0, nullptr);

	return GST_FLOW_OK;
}


// URI : "udp://192.168.1.1:5600"
GstAppPipeline::GstAppPipeline(u32 pipelineID, const GstApp* gstApp, u32 width, u32 height, char* uri)
	: id(pipelineID), app(gstApp), textureWidth(width), textureHeight(height)
{
	mainLoop = nullptr;
	pipelineLoopThread = nullptr;
	texture = nullptr;
	sharedHandle = nullptr;


	// Create pipeline
	auto pipelineName = std::string("udp-d3d11-decoder-") + std::to_string(id);
	pipeline = gst_pipeline_new(pipelineName.c_str());

	// Create pipeline elements
	GstElement* udpsrc = gst_element_factory_make("udpsrc", NULL);
	GstElement* rtph264depay = gst_element_factory_make("rtph264depay", NULL);
	GstElement* h264parse = gst_element_factory_make("h264parse", NULL);
	GstElement* d3d11h264dec = gst_element_factory_make("d3d11h264dec", NULL);
	GstElement* d3d11convert = gst_element_factory_make("d3d11convert", NULL);
	GstElement* d3d11videosink = gst_element_factory_make("d3d11videosink", NULL);
	if (!pipeline || !udpsrc || !rtph264depay || !h264parse || !d3d11h264dec || !d3d11convert || !d3d11videosink) {
		Debug::Log("Failed to create all elements");
		gst_object_unref(pipeline);
		return;
	}

	// Configure the pipeline:
	g_object_set(G_OBJECT(udpsrc), "uri", uri, "caps", gst_caps_from_string("application/x-rtp"), NULL);
	g_object_set(G_OBJECT(d3d11videosink), "draw-on-shared-texture", true, NULL);
	g_signal_connect(G_OBJECT(d3d11videosink), "begin_draw", G_CALLBACK(OnBeginDraw), this);

	// Build and link the pipeline
	gst_bin_add_many(GST_BIN(pipeline), udpsrc, rtph264depay, h264parse, d3d11h264dec, d3d11convert, d3d11videosink, NULL);
	if (!gst_element_link_many(udpsrc, rtph264depay, h264parse, d3d11h264dec, d3d11convert, d3d11videosink, NULL)) {
		Debug::Log("Elements could not be linked. Exiting.");
		gst_object_unref(pipeline);
		return;
	}

	// Launch the pipeline
	auto state = gst_element_set_state(pipeline, GstState::GST_STATE_PLAYING);
	if (state == GstStateChangeReturn::GST_STATE_CHANGE_FAILURE) {
		gst_object_unref(pipeline);
		return;
	}

	// Create the loop thread?
	pipelineLoopThread = g_thread_new("GstUnityBridge Main Thread", GstMainLoopFunction, this);
	if (!pipelineLoopThread) {
		Debug::Log("Failed to create GLib main thread: ");
		return;
	}
}


GstAppPipeline::~GstAppPipeline() {
	g_main_loop_quit(mainLoop);
	mainLoop = nullptr;
	g_thread_join(pipelineLoopThread);
	pipelineLoopThread = nullptr;
	gst_element_set_state(pipeline, GstState::GST_STATE_NULL);
	gst_object_unref(pipeline);
}


HANDLE GstAppPipeline::GetSharedHandle() {
	return sharedHandle;
}


ID3D11Texture2D* GstAppPipeline::GetTexturePtr() {
	return texture;
}


// Call only on plugin thread
// Creates the underlying D3D11 texture using the provided unity device.
// This texture can then be turned into a proper Unity texture on the
// managed side using Texture2D.CreateExternalTexture()
bool GstAppPipeline::CreateTexture()
{
	auto device = app->getD3D11Device();
	HRESULT hr = S_OK;

	// Create a texture 2D that can be shared
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = textureWidth;
	desc.Height = textureHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;

	// Create the texture using the device and description
	hr = device->CreateTexture2D(&desc, nullptr, &texture);
	if (FAILED(hr)) {
		Debug::Log("Could not create texture.");
		return false;
	}

	// Cast the pointer into IDXGIRes. that can be shared.
	IDXGIResource* pDXGITexture = nullptr;
	hr = texture->QueryInterface(__uuidof(IDXGIResource), (void**)&pDXGITexture);
	if (FAILED(hr)) {
		Debug::Log("Couldnt DXGI res");
		return false;
	}

	// Create a shared handle (so the GStreamer thread can write to this)
	HANDLE sharedHandle = nullptr;
	hr = pDXGITexture->GetSharedHandle(&sharedHandle);
	if (FAILED(hr)) {
		Debug::Log("Couldnt create shared handle");
		pDXGITexture->Release();
		return false;
	}
	this->sharedHandle = sharedHandle;

	Debug::LogNoNewLine("Created texture for pipeline:");
	Debug::Log(static_cast<int>(id));

	return true;
}


// Call only on plugin thread
void GstAppPipeline::ReleaseTexture(ID3D11Texture2D* texture) {
	if (texture == nullptr) return;
	texture->Release();
	texture = nullptr;
}