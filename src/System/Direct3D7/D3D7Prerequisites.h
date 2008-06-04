#	pragma once

//#include "OgrePrerequisites.h"
#	include "Config/Config.h"

// Define versions for if DirectX is in use (Win32 only)
#define DIRECT3D_VERSION  0x0700
#define DIRECTINPUT_VERSION 0x0700
#define DIRECTDRAW_VERSION 0x0700

// Include D3D and DirectDraw stuff
#define D3D_OVERLOADS
#define NOMINMAX // required to stop windows.h screwing up std::min definition
#include <ddraw.h>
#include <d3d.h>
#include <d3dx.h>
#include <d3dxcore.h>
#include "d3dutil.h"


namespace Menge
{
    template< typename _Interf >
    inline void __safeRelease( _Interf **interf ) throw ()
    {
        if( *interf )
        {
            (*interf)->Release();
            (*interf) = NULL;
        }
    }

    // Predefine classes
    class D3DDevice;
    class D3DDeviceList;
    class D3DRenderSystem;
    class D3DTexture;
    class D3DTextureManager;
    class DDDriver;
    class DDDriverList;
    class DDVideoMode;
    class DDVideoModeList;

	//-------------------------------------------
	// Windows setttings
	//-------------------------------------------
#if (MENGE_PLATFORM == MENGE_PLATFORM_WINDOWS) && !defined(MENGE_STATIC_LIB)
#	ifdef OGRED3DENGINEDLL_EXPORTS
#		define _MengeD3DExport __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define _MengeD3DExport
#       else
#    		define _MengeD3DExport __declspec(dllimport)
#       endif
#	endif
#else
#	define _MengeD3DExport
#endif	// MENGE_PLATFORM_WINDOWS


}
