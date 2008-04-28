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
#include "OgreDDDriverList.h"
#include "OgreDDDriver.h"
#include "OgreLogManager.h"
#include "OgreException.h"

namespace Ogre {

    // Non-member callback functions
    static BOOL WINAPI DDEnumCallbackEx(GUID FAR *lpGUID,
                    LPSTR     lpDriverDescription,
                    LPSTR     lpDriverName,
                    LPVOID    lpContext,
                    HMONITOR  hm)
    {

        DDDriverList* driverList;

        // Use context pointer to add this new object to the list
        driverList = (DDDriverList*)lpContext;
        driverList->AddDriver(lpGUID, lpDriverDescription, lpDriverName);


        // Continue enumeration (to find other cards)
        return TRUE;
    }

    static BOOL WINAPI DDEnumCallback(GUID FAR *lpGUID,
                               LPSTR  lpDriverDescription,
                               LPSTR  lpDriverName,
                               LPVOID lpContext)
    {
        return DDEnumCallbackEx(lpGUID,lpDriverDescription,lpDriverName,lpContext,NULL);
    }

    DDDriverList::DDDriverList()
    {
        // Default constructor
        // Will create a  new driver list and enumerate it

        // Enumerate the list
        enumerate();

    }

    DDDriverList::~DDDriverList()
    {
		for(size_t i=0; i<count(); i++)
		{
			item(i)->Cleanup();
		}
        // Release drivers
        mDriverList.clear();
    }

    BOOL DDDriverList::enumerate()
    {

        LPDIRECTDRAWENUMERATEEX lpDDEnumEx;
        HINSTANCE h;

        LogManager::getSingleton().logMessage("----- DirectDraw Detection Starts");

        h = LoadLibrary("ddraw.dll");

        // If ddraw.dll doesn't exist in the search path,
        // then DirectX probably isn't installed, so fail.
        if (!h)
            throw Exception(0, "Error loading ddraw.dll", "DDDriverList - enumerate");

        // Note that you must know which version of the
        // function to retrieve (see the following text).
        // For this example, we use the ANSI version.
        lpDDEnumEx = (LPDIRECTDRAWENUMERATEEX) GetProcAddress(h,"DirectDrawEnumerateExA");
         // If the function is there, call it to enumerate all display
        // devices attached to the desktop, and any non-display DirectDraw
        // devices.
        if (lpDDEnumEx)
            lpDDEnumEx(DDEnumCallbackEx, this,
                DDENUM_ATTACHEDSECONDARYDEVICES |
                DDENUM_NONDISPLAYDEVICES             );
        else
        {
            /*
             * We must be running on an old version of DirectDraw.
             * Therefore MultiMon isn't supported. Fall back on
             * DirectDrawEnumerate to enumerate standard devices on a
             * single-monitor system.
            */
            DirectDrawEnumerate(DDEnumCallback,this);

        }
        // If the library was loaded by calling LoadLibrary(),
        // then you must use FreeLibrary() to let go of it.

        FreeLibrary(h);

        LogManager::getSingleton().logMessage("----- DirectDraw Detection Ends");

        return TRUE;
    }

    void DDDriverList::AddDriver(
        GUID FAR *lpGuid,
        LPSTR lpDriverDesc,
        LPSTR lpDriverName )
    {
        mDriverList.push_back( DDDriver(lpGuid, lpDriverDesc, lpDriverName) );

        LogManager::getSingleton().logMessage( LML_NORMAL, 
            String("Detected DirectDraw driver ") + lpDriverDesc );
    }

    size_t DDDriverList::count(void) const
    {
        return mDriverList.size();
    }

    DDDriver* DDDriverList::item(size_t index)
    {
        return &mDriverList.at( index );
    }
}
