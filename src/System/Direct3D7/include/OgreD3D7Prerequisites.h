/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2005 The OGRE Team
Also see acknowledgements in Readme.html

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/

#ifndef _D3DPrerequisites_H__
#define _D3DPrerequisites_H__

#include "OgrePrerequisites.h"

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


namespace Ogre {

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
#if (OGRE_PLATFORM == OGRE_PLATFORM_WIN32) && !defined(OGRE_STATIC_LIB)
#	ifdef OGRED3DENGINEDLL_EXPORTS
#		define _OgreD3DExport __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define _OgreD3DExport
#       else
#    		define _OgreD3DExport __declspec(dllimport)
#       endif
#	endif
#else
#	define _OgreD3DExport
#endif	// OGRE_WIN32


}


#endif
