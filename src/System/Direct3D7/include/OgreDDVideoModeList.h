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
#ifndef __DDVIDEOMODELIST_H__
#define __DDVIDEOMODELIST_H__

// Precompiler options
#include "OgreD3D7Prerequisites.h"

namespace Ogre {
    class DDVideoMode;

    /**
      Encapsulates enumeration of DirectDraw video modes. Utility class for D3DRenderSystem.

     */
    class DDVideoModeList
    {

    private:

        // Pointer to DDD interface
        LPDIRECTDRAW7 lpDD7;

        // STL list of drivers (Vector used to allow random access)
        std::vector<DDVideoMode> mModeList;

        void AddMode(LPDDSURFACEDESC2 lpDDSurfaceDesc);


    public:
        DDVideoModeList(LPDIRECTDRAW7 lpDD7); // Only allowed Constructor
        ~DDVideoModeList(); // Destructor
        BOOL enumerate(void); //Forces an enumeration / re-enumeration

        // List accessors
        DDVideoMode* item(int index);
        unsigned int count(void);
        DDVideoMode* getClosestMatch(int width, int height, int colourDepth);



        // Non-member callbacks
        friend static HRESULT CALLBACK DDEnumModesCallback(
                                    LPDDSURFACEDESC2 lpDDSD2,
                                    LPVOID lpContext);

    };
} // namespace Ogre
#endif
