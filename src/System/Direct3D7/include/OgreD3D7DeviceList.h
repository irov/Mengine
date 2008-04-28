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
#ifndef __D3DDEVICELIST_H__
#define __D3DDEVICELIST_H__

// Precompiler options
#include "OgreD3D7Prerequisites.h"

namespace Ogre {

    class _OgreD3DExport D3DDevice;
    /**
      Encapsulates enumeration of Direct3D devices. Utility class for D3DRenderSystem.
     */
    class  _OgreD3DExport D3DDeviceList
    {

    private:


        // STL list of drivers (Vector used to allow random access)
        std::vector<D3DDevice> mDeviceList;

        void AddDevice(    LPSTR lpDeviceDesc,
                        LPSTR lpDeviceName,
                        LPD3DDEVICEDESC7 lpD3DDeviceDesc);


    public:
        // Pointer to D3D interface
        LPDIRECT3D7 lpD3D;


        D3DDeviceList(LPDIRECT3D7 lpD3D); // Only allowed Constructor
        ~D3DDeviceList();
        BOOL enumerate(void); //Forces an enumeration / re-enumeration

        // List accessors
        D3DDevice* item(size_t index);
        size_t count(void);

        /**
          Retrieves the 'best' 3D device type

          This method returns the device type that is 'best' out of those available. The 3 types
          that might be present are software emulation, hardware accelerated (HAL), and hardware transform
          and light. Generally the latter are preferred over the former, however in some cases (mainly
          very low-end cards) the hardware accelerated device type supports less features than the software
          mode so may be preferred. In addition some HALs don't support the full range of rendering bit depths
          (e.g. Matrox Millennium 1/2) so if a 32-bit colour depth is chosen, software mode must be used.
        */
        D3DDevice* getBest(unsigned int minColourDepth);


        // Non-member callbacks
        friend static HRESULT CALLBACK D3DEnumDevicesCallback(
                                    LPSTR lpDeviceDescription,
                                    LPSTR lpDeviceName,
                                    LPD3DDEVICEDESC7 lpD3DDeviceDesc,
                                    LPVOID lpContext);
    };

}// Namespace Ogre
#endif
