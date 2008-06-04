
#include "D3D7RenderSystem.h"
//#include "OgreRoot.h"

namespace Menge 
{

    D3DRenderSystem* d3dRendPlugin;
    //-----------------------------------------------------------------------
    extern "C" void dllStartPlugin(void) throw()
    {
        // Create d3d rendering api

        // Must get HISTANCE
        HINSTANCE hInst = GetModuleHandle("RenderSystem_Direct3D7.dll");

        d3dRendPlugin = new D3DRenderSystem(hInst);

        // Register the render system
        Root::getSingleton().addRenderSystem(d3dRendPlugin);

    }

    //-----------------------------------------------------------------------
    extern "C" void dllStopPlugin(void)
    {
        delete d3dRendPlugin;
    }

}

