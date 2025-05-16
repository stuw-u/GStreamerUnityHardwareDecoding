# GStreamerUnityHardwareDecoding
Decode UDP video streams using hardware decoding with DX11, using a native plugin GStreamer application.

## Setup
1. [Install GStreamer](https://gstreamer.freedesktop.org/download/). The version 1.22.6 was used in this project but any version that supports DX11 commands should work. Install MSVC 64bit version, both runtime only. I recommend doing a full install.
2. Setup Path Variables for gstreamer: Windows start icon > Search "environment variables" > Edit the system environment variables > Environment Variables > System variables > Variable :Path > Edit > New > Paste "C:\gstreamer\1.0\msvc_x86_64\bin" > Ok
3. This project was made on Unity 2023.2, but any version after 2020 should be supported. You don't need to include the visual studio in this project, everything you need is in the UnityProject/Plugin folder
4. Add a GStreamerSingleton in the scene and a GStreamerVideoPlayer. The video resolution doesn't matter.
5. You have to play around with the firewall settings to let the UPD packages throught! Google "allow UPD packets inbound in control panel".

## Setup (Compiling the plugin)
1. Install the MSVC development installer from the same link and version you downloaded the runtime installer.
2. Setup System Variable: Windows start icon > Search "environment variables" > Edit the system environment variables > Environment Variables > System variables > New... then set "Variable name" to "GSTREAMER_1_0_ROOT_MSVC_X86_64" and "Variable value" to "C:\gstreamer\1.0\msvc_x86_64\" 

## Limitations
- Only supports DX11
- Only supports UDP streams
- Only supports 64bit application
- Only supports hardware decoding, no CPU option.

Adding support for different graphic api should not be toooo hard. Everything that would need to be modified is the plugin itself, adding the necessary IUnityInterfaces for your graphic api, then change the texture creating/dispose function. Don't forget to modify the pipeline to also use the decoding/display keywords. Hope that helps?

![Showcase of a stream being displayed in unity](https://i.imgur.com/Ei2E8kl.png)
