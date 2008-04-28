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
#ifndef __DDDRIVER_H__
#define __DDDRIVER_H__

// Precompiler options
#include "OgreD3D7Prerequisites.h"

// Declaration
#include "OgreString.h"

namespace Ogre {

    class D3DDeviceList;
    class DDVideoModeList;
    class DDVideoMode;
    class D3DDevice;


    /**
      Encapsulates a DirectDraw driver. Utility class for D3DRenderSystem.
     */
    class _OgreD3DExport DDDriver
    {
    private:

        String mDriverName;
        String mDriverDesc;
        GUID  mGuid; // Note - copy of GUID, not pointer to
        bool  mPrimaryDisplay;

        LPDIRECTDRAW7 lpDD7; // pointer to IDirectDraw7 interface
        LPDIRECT3D7   lpD3D; // pointer to IDirect3D7 interface (if required)
        D3DDeviceList* mDeviceList; // List of Direct3D devices
        DDVideoModeList* mVideoModeList; // List of video modes
        D3DDevice* active3DDevice;
        DDVideoMode* activeVideoMode;
        HWND activeHWnd;

        DDCAPS mSWCaps; // Software capabilities
        DDCAPS mHWCaps; // Hardware capabilities
        void logCaps(void) const;

        LPDIRECTDRAWSURFACE7 lpDDSPrimary; // Pointer to primary surface
        LPDIRECTDRAWSURFACE7 lpDDSBack; // Pointer to back buffer

        bool runningFullScreen;
        bool using3DMode;

        RECT rcViewport; //Only used for windowed mode
        LPDIRECTDRAWCLIPPER lpDDClipper;

        // Private accessor functions

        void RestoreSurfaces(void);



    public:
        // Constructors
        DDDriver(); // Default
        DDDriver(const DDDriver &ob); // Copy
        DDDriver(GUID FAR *lpGuid,    // Create from enum
                                LPSTR lpDriverDescription,
                                LPSTR lpDriverName);
        // Destructor
        ~DDDriver();

        /** Creates DirectDraw surfaces appropriate for a render window.

			@remarks
				This method creates double-buffered surfaces for a window. Note that in this case the
				metrics supplied (width, height) must be of the CLIENT area of a window only
				if fullScreen = false; i.e. the metrics must be ignoring title bar, menu bar etc.

			@param hWnd 
				Window handle to the window for which the surfaces are created.
			@param width 
				The width of the surfaces required (see note above).
			@param height 
				The height of the surfaces required (see note above).
			@param colourDepth 
				The colour depth in bits per pixel. Only relevant if fullScreen = true.
			@param fullScreen 
				Specify true to make these surfaces full screen exclusive.
			@param front 
				Pointer to a pointer which will be filled with the interface to the front buffer on return.
			@param back 
				Pointer to a pointer which will be filled with the interface to the back buffer on return.
				Only relevant if fullScreen = false.
         */
        void createWindowSurfaces(HWND hWnd, unsigned int width, unsigned int height, unsigned int colourDepth, bool fullScreen,
            LPDIRECTDRAWSURFACE7 *front, LPDIRECTDRAWSURFACE7 *back);

        void Cleanup(void);
        void CheckWindow(void);

        // Overloaded operator =
        DDDriver operator=(const DDDriver &orig);

        // Information accessors
        String DriverName(void) const;
        String DriverDescription(void) const;
        LPDIRECTDRAW7 directDraw(); // Gets lpDD7 (instantiates if required)

        D3DDeviceList* get3DDeviceList(void);
        DDVideoModeList* getVideoModeList(void);
        DDVideoMode* getActiveVideoMode(void);

        // Retrieval of other objects
        D3DDevice* get3DDevice(void);

        void OutputText(int x, int y, char* text);
        void FlipBuffers(void);
        bool RunningFullScreen(void) const;
        RECT ViewportRect(void) const;

        // Capabilities
        bool Has3DAcceleration(void) const;
        bool CanRenderWindowed(void) const;

        // Generalised info
        void GetDisplayDetails(unsigned int& width, unsigned int& height, unsigned int& colourDepth);
    };
}

#endif
