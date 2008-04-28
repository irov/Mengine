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
#ifndef __DDDRIVERLIST_H__
#define __DDDRIVERLIST_H__

// Precompiler options
#include "OgreD3D7Prerequisites.h"

namespace Ogre {
    /**
      Encapsulates enumeration of DirectDraw drivers. Utility class for D3DRenderSystem.
      */
    class DDDriverList
    {

    private:
        // STL list of drivers (Vector used to allow random access)
        std::vector<DDDriver> mDriverList;

        void AddDriver(GUID FAR *lpGuid,
                                LPSTR lpDriverDesc,
                                LPSTR lpDriverName);


    public:
        DDDriverList(); // Default Constructor
        ~DDDriverList(); // Default desctructor
        BOOL enumerate(void); //Forces an enumeration / re-enumeration

        /** Returns the item at the specified position. Throws an
            std::out_of_range if the index is greater then the size.
        */
        DDDriver* item(size_t index);

        size_t count(void) const;



        // Non-member callbacks
        friend static BOOL WINAPI DDEnumCallbackEx(GUID FAR *lpGUID,
                        LPSTR     lpDriverDescription,
                        LPSTR     lpDriverName,
                        LPVOID    lpContext,
                        HMONITOR  hm);


        friend static BOOL WINAPI DDEnumCallback(GUID FAR *lpGUID,
                                   LPSTR  lpDriverDescription,
                                   LPSTR  lpDriverName,
                                   LPVOID lpContext);


    };
} // Namespace Ogre

#endif
