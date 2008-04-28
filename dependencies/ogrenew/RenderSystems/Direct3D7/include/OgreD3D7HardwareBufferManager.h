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

#ifndef __D3D7HardwareBufferManager_H__
#define __D3D7HardwareBufferManager_H__

#include "OgrePrerequisites.h"
#include "OgreDefaultHardwareBufferManager.h"
#include "OgreException.h"

namespace Ogre {


	/** Specialisation of HardwareBufferManager for D3D7.
	@remarks
        This class uses the software emulated buffers because D3D7's
        hardware vertex buffers are far too primitive. The only reason we
        override this is because D3D7 only allows 16-bit indexes so we have 
        to prevent 32-bit index buffers being created.
	*/
	class D3D7HardwareBufferManager : public DefaultHardwareBufferManager
	{
    public:
        D3D7HardwareBufferManager() {}
        ~D3D7HardwareBufferManager() {}
		/// Create a hardware index buffer
		HardwareIndexBufferSharedPtr 
            createIndexBuffer(HardwareIndexBuffer::IndexType itype, size_t numIndexes, 
				HardwareBuffer::Usage usage, bool useShadowBuffer = false)
        {
            if (itype == HardwareIndexBuffer::IT_32BIT)
            {
                OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Direct3D7 only supports 16-bit index buffers, "
                    "you should upgrade to a more recent Direct3D RenderSystem if you wish to use "
                    "32-bit indexes.", "D3D7HardwareBufferManager::createIndexBuffer");
            }
            return DefaultHardwareBufferManager::createIndexBuffer(itype, numIndexes, usage, useShadowBuffer);
        }

    };


}

#endif
