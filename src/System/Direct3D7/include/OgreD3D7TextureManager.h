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
#ifndef _D3DTextureManager_H__
#define _D3DTextureManager_H__

#include "OgreD3D7Prerequisites.h"

#include "OgreTextureManager.h"
#include "OgreD3D7Texture.h"


namespace Ogre {
    /** Direct3D-specific implementation of a TextureManager */
    class D3DTextureManager : public TextureManager
    {
    public:
        D3DTextureManager(LPDIRECT3DDEVICE7 lpD3DDevice);
        virtual ~D3DTextureManager();

		/** Restore the contents of all surfaces following a lost device. */
		void reloadAfterLostDevice(void);

		PixelFormat getNativeFormat(TextureType ttype, PixelFormat format, int usage);

		bool isHardwareFilteringSupported(TextureType ttype, PixelFormat format, int usage,
			bool preciseFormatOnly = false);


    protected:
        /// @copydoc ResourceManager::createImpl
        Resource* createImpl(const String& name, ResourceHandle handle, 
            const String& group, bool isManual, ManualResourceLoader* loader, 
            const NameValuePairList* createParams);

        LPDIRECT3DDEVICE7 mlpD3DDevice;
    };
}
#endif
