#pragma once
#include "DebugCpp.h"

#define GST_USE_UNSTABLE_API
#define UnityExport extern "C" __declspec(dllexport)
#define EVENT_CREATE_TEXTURE 1
#define EVENT_DISPOSE_TEXTURE 2

typedef unsigned char u8;
typedef unsigned int u32;
typedef int i32;

