using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace GStreamerUnity
{
    public class GStreamerPlayer : MonoBehaviour
    {
        [SerializeField] Renderer target;
        [SerializeField] private string udpUri = "192.168.1.1:5600";
        [SerializeField] private uint width = 1920;
        [SerializeField] private uint height = 1080;

        bool createdMaterial;
        GStreamerPipeline pipeline;

        private void Start ()
        {
            pipeline = GStreamerPipeline.Create(width, height, "udp://" + udpUri);
            pipeline.CreateTexture();
        }

        private void Update ()
        {
            if (createdMaterial) return;
            var texture = pipeline.GetTexture();
            if (texture != null)
            {
                target.material.SetTexture("_MainTex", texture);
                createdMaterial = true;
            }
        }

        private void OnDestroy ()
        {
            pipeline.Dispose();
        }
    }
}

