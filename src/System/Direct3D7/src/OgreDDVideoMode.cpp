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
#include "OgreD3D7RenderSystem.h"
#include "OgreDDVideoMode.h"

namespace Ogre {
    // Constructors
    DDVideoMode::DDVideoMode()
    {
        // Nothing
    }

    // Copy Constructor
    DDVideoMode::DDVideoMode(const DDVideoMode &ob)
    {
        mWidth = ob.mWidth;
        mHeight = ob.mHeight;
        mColourDepth = ob.mColourDepth;
        mDDSurfaceDesc = ob.mDDSurfaceDesc;
    }

    // Enum constructor
    DDVideoMode::DDVideoMode(LPDDSURFACEDESC2 lpDDSD2)
    {
        // Copy surface desc
        memcpy(&mDDSurfaceDesc, lpDDSD2, sizeof(DDSURFACEDESC2));

        // Determine basic information
        mWidth = mDDSurfaceDesc.dwWidth;
        mHeight = mDDSurfaceDesc.dwHeight;
        mColourDepth = mDDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount;


    }
    DDVideoMode DDVideoMode::operator=(const DDVideoMode &orig)
    {

        mWidth = orig.mWidth;
        mHeight = orig.mHeight;
        mColourDepth = orig.mColourDepth;
        mDDSurfaceDesc = orig.mDDSurfaceDesc;


        return *this;

    }

    String DDVideoMode::Description(void)
    {

        char tmp[128];
        char *final;

        sprintf(tmp, "%d x %d @ %d-bit colour", mWidth, mHeight, mColourDepth);
        return String(tmp);

/*        final = (char*)malloc(sizeof(char)*(strlen(tmp)+1));

        strcpy(final, tmp);*/

        return final;

    }
}
