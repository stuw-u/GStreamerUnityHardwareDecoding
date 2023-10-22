using System;
using UnityEngine;

namespace GStreamerUnity
{
    public class GStreamerPipeline : IDisposable
    {
        public IntPtr Ptr { get; private set; }
        public IntPtr TextureNativePtr { get; private set; }
        private uint id;
        private uint width;
        private uint height;
        static uint _totalId;
        Texture2D texture;

        private GStreamerPipeline() { }

        public static GStreamerPipeline Create (uint width, uint height, string uri)
        {
            var id = _totalId++;
            return new GStreamerPipeline
            {
                width = width,
                height = height,
                Ptr = GStreamerInterop.CreatePipeline(id, width, height, uri),
                id = id
            };
        }

        public void CreateTexture ()
        {
            GStreamerInterop.CreateTexture(this);
        }

        public Texture2D GetTexture ()
        {
            if (texture == null)
            {
                TextureNativePtr = GStreamerInterop.GetTexturePtr(this);

                if (TextureNativePtr != IntPtr.Zero)
                {
                    texture = Texture2D.CreateExternalTexture((int)width, (int)height, TextureFormat.RGBA32, mipChain: false, linear: true, TextureNativePtr);
                    return texture;
                }
            }
            return texture;
        } 

        public void Dispose ()
        {
            if(TextureNativePtr != IntPtr.Zero)
            {
                GStreamerInterop.DisposeTexture(TextureNativePtr);
                TextureNativePtr = IntPtr.Zero;
            }

            GStreamerInterop.DisposePipeline(Ptr);
            Ptr = IntPtr.Zero;

            if (texture)
            {
                UnityEngine.Object.Destroy(texture);
                texture = null;
            }
        }
    }
}
