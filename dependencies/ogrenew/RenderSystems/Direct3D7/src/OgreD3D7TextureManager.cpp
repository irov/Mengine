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
#include "OgreD3D7TextureManager.h"
#include "OgreD3D7Texture.h"

namespace Ogre {

/* Auto-inlining must be disabled for this method otherwise problems with the 
   DLL boundary occur on the constructor in release mode. */
#if OGRE_COMPILER == OGRE_COMPILER_MSVC
    #pragma auto_inline( off )
#endif

    D3DTextureManager::D3DTextureManager(LPDIRECT3DDEVICE7 lpD3D)
        : TextureManager()
    {
        mlpD3DDevice = lpD3D;
        lpD3D->AddRef();

        // register with group manager
        ResourceGroupManager::getSingleton()._registerResourceManager(mResourceType, this);
    }

#if OGRE_COMPILER == OGRE_COMPILER_MSVC
    #pragma auto_inline( on )
#endif

    //-----------------------------------------------------------------------
    D3DTextureManager::~D3DTextureManager()
    {
        __safeRelease( &mlpD3DDevice );
        // unregister with group manager
        ResourceGroupManager::getSingleton()._unregisterResourceManager(mResourceType);
    }
    //-----------------------------------------------------------------------
    Resource* D3DTextureManager::createImpl(const String& name, 
        ResourceHandle handle, const String& group, bool isManual, 
        ManualResourceLoader* loader, const NameValuePairList* createParams)
    {
        return new D3DTexture(this, name, handle, group, isManual, loader, mlpD3DDevice);
    }
	//-----------------------------------------------------------------------
	void D3DTextureManager::reloadAfterLostDevice(void)
	{
		// We've been restored after a lost device and need to repopulate the
		// texture data
		for (ResourceMap::iterator i = mResources.begin(); i != mResources.end(); ++i)
		{
			D3DTexturePtr t = i->second;
			t->restoreFromLostDevice();
		}
	}
	//-----------------------------------------------------------------------
	PixelFormat D3DTextureManager::getNativeFormat(TextureType ttype, PixelFormat format, int usage)
	{
		// Basic filtering
		//D3DFORMAT d3dPF = D3D9Mappings::_getPF(D3D9Mappings::_getClosestSupportedPF(format));
		

		// Calculate usage
		/*DWORD d3dusage = 0;
		D3DPOOL pool = D3DPOOL_MANAGED;
		if (usage & TU_RENDERTARGET) 
		{
			d3dusage |= D3DUSAGE_RENDERTARGET;
			pool = D3DPOOL_DEFAULT;
		}
		if (usage & TU_DYNAMIC)
		{
			d3dusage |= D3DUSAGE_DYNAMIC;
			pool = D3DPOOL_DEFAULT;
		}

		// Use D3DX to adjust pixel format
		switch(ttype)
		{
		case TEX_TYPE_1D:
		case TEX_TYPE_2D:
			D3DXCheckTextureRequirements(mpD3DDevice, NULL, NULL, NULL, d3dusage, &d3dPF);
			break;
		case TEX_TYPE_3D:
			D3DXCheckVolumeTextureRequirements(mpD3DDevice, NULL, NULL, NULL, NULL, d3dusage, &d3dPF, pool);
			break;
		case TEX_TYPE_CUBE_MAP:
			D3DXCheckCubeTextureRequirements(mpD3DDevice, NULL, NULL, d3dusage, &d3dPF, pool);
			break;
		};*/

		//return D3D9Mappings::_getPF(d3dPF);
		return format;
	}
	//-----------------------------------------------------------------------
	bool D3DTextureManager::isHardwareFilteringSupported(TextureType ttype, PixelFormat format, int usage, bool preciseFormatOnly)
	{
		/*if (!preciseFormatOnly)
			format = getNativeFormat(ttype, format, usage);

		D3D9RenderSystem* rs = static_cast<D3D9RenderSystem*>(
			Root::getSingleton().getRenderSystem());

		return rs->_checkTextureFilteringSupported(ttype, format, usage);*/
		return false;
	}
	//-----------------------------------------------------------------------
}
