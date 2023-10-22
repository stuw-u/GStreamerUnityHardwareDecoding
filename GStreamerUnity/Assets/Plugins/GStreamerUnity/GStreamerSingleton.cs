using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// TODO: This singleton format is not ideal.
// TODO: Somethings other than a Render should probably be provided
// TODO: Texture size is hardcoded. Consider getting it from RTP stream
// TODO: Look into multiple stream support
// TODO: Consider moving some of the code here to the GStreamer Player?

namespace GStreamerUnity
{
    [DefaultExecutionOrder(-1)]
    public class GStreamerSingleton : MonoBehaviour
    {
        
        private static GStreamerSingleton inst;
        

        private void OnEnable ()
        {
            if (inst != null)
            {
                Debug.LogError("Duplicate GStreamer Singleton!");
                Destroy(gameObject);
            }
            inst = this;
        }

        private void OnDisable ()
        {
            if (inst != null)
            {
                inst = null;
                GStreamerInterop.PrintLog();
            }
        }

        private void Update ()
        {
            if (inst == null) return;
            GStreamerInterop.PrintLog();
        }
    }
}
