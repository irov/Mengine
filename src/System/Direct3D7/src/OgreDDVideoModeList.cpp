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
#include "OgreDDVideoModeList.h"
#include "OgreDDVideoMode.h"
#include "OgreException.h"

namespace Ogre {

    // Non-member callback functions
    static HRESULT CALLBACK DDEnumModesCallback(
                                    LPDDSURFACEDESC2 lpDDSD2,
                                    LPVOID lpContext)
    {
        DDVideoModeList* modeList;

        // Omit any palettised modes
        if (lpDDSD2->ddpfPixelFormat.dwRGBBitCount >= 16)
        {

            modeList = (DDVideoModeList*) lpContext;

            modeList->AddMode(lpDDSD2);
        }

        // Continue enumeration
        return DDENUMRET_OK;
    }


    DDVideoModeList::DDVideoModeList(LPDIRECTDRAW7 directDraw)
    {
        // Will create a  new driver list and enumerate it
        if (directDraw == NULL)
            throw Exception(0, "NULL supplied as "
                "DD interface pointer.", "DDVideoModeList Contructor");

        lpDD7 = directDraw;
        // Enumerate the list
        enumerate();

    }

    DDVideoModeList::~DDVideoModeList()
    {
        // Release each video mode object
        std::vector<DDVideoMode>::iterator p = mModeList.begin();

        mModeList.erase(p, p+mModeList.size()-1);
    }




    BOOL DDVideoModeList::enumerate()
    {
        HRESULT hr;

        // Enumerate display modes
        // Different refresh rates are NOT enumerated (dwFlags = 0)
        hr = lpDD7->EnumDisplayModes(0, NULL, this, DDEnumModesCallback);
        if (FAILED(hr))
            throw Exception(0, "Error enumerating display modes", "DDVideoModeList - enumerate");


        return TRUE;
    }

    void DDVideoModeList::AddMode(LPDDSURFACEDESC2 lpDDSD2)
    {
        //DDVideoMode *newMode;


        // Create new mode
        //newMode = new DDVideoMode(lpDDSD2);

        // Add it to my list
        mModeList.push_back(DDVideoMode(lpDDSD2)/**newMode*/);


    }

    unsigned int DDVideoModeList::count(void)
    {
        return static_cast< unsigned int >( mModeList.size() );
    }

    DDVideoMode* DDVideoModeList::item(int index)
    {
        // Get an iterator for the vector
        std::vector<DDVideoMode>::iterator p = mModeList.begin();

        // Return the indexed driver
        return &p[index];


    }

    DDVideoMode* DDVideoModeList::getClosestMatch(int width, int height, int colourDepth)
    {
        // Search through looking for closest match
        int bestDiff, currentDiff, bestIndex;
        DDVideoMode *vm;

        std::vector<DDVideoMode>::iterator p = mModeList.begin();


        bestDiff = 9999;
        bestIndex = -1;
        for( unsigned j = 0; j < count(); j++ )
        {
            currentDiff = 0;
            vm = &p[j];
            currentDiff += abs(vm->mWidth - width);
            currentDiff += abs(vm->mHeight - height);
            currentDiff += abs(vm->mColourDepth - colourDepth);

            if (currentDiff < bestDiff)
            {
                bestDiff = currentDiff;
                bestIndex = j;
            }

            /* We Love Intel's Compilers :) */
            if (currentDiff == 0)
                break; // No point continuing, direct match
        }

        return &p[bestIndex];
    }

}


