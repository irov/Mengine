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
#ifndef __D3D7RenderWindow_H__
#define __D3D7RenderWindow_H__

// Precompiler options
#include "OgreD3D7Prerequisites.h"

#include "OgreRenderWindow.h"
#include "OgreDDDriver.h"
#include "OgreD3D7DeviceList.h"
#include "OgreD3D7Device.h"

namespace Ogre {
    /** Defines the rendering window on Win32 systems
        @remarks
            This class represents the rendering window on the
            Windows family of operating systems.
     */
    class D3D7RenderWindow : public RenderWindow
    {
    public:
        D3D7RenderWindow(HINSTANCE instance, DDDriver *driver);
        ~D3D7RenderWindow();

		void create(const String& name, unsigned int width, unsigned int height,
			bool fullScreen, const NameValuePairList *miscParams);

        /** Overridden - see RenderWindow 
        */
        void destroy(void);

        /** Overridden - see RenderWindow
        */
        bool isClosed(void) const;

        /** Overridden - see RenderWindow
        */
        void reposition(int left, int top);

        /** Overridden - see RenderWindow
        */
        void resize(unsigned int width, unsigned int height);

        /** Overridden - see RenderWindow
        */
        void swapBuffers(bool waitForVSync);

        /** Retrieves a handle to the window (Win32 specific)
        */
        HWND getWindowHandle(void) const;

        /** Retrieves a handle to the window's parent, or 0 if there is no parent.
        */
        HWND getParentWindowHandle(void) const;

        /** Returns true if this window is using DirectDraw.
        */
        bool isUsingDirectDraw(void) const;
        /** Retrieves a pointer to the DirectDraw wrapper object used to 
            manage the window's surface. (Win32 specific)
            @note
                Only applicable for windows using DirectDraw.
        */
        DDDriver* getDirectDrawDriver(void);

        /** Returns a pointer to a DirectDraw surface containing the
            front buffer.
            @note
                Only applicable for windows using DirectDraw.
        */
        LPDIRECTDRAWSURFACE7 getDDFrontBuffer(void);

        /** Returns a pointer to a DirectDraw surface containing the back 
            buffer. Only applicable if using DirectDraw.
        */
        LPDIRECTDRAWSURFACE7 getDDBackBuffer(void);

        /** Returns a pointer to the Direct3D device that works with this
            window
            @note
                Only applicable for windows using DirectDraw.
        */
        LPDIRECT3DDEVICE7 getD3DDevice(void);

        bool requiresTextureFlipping() const { return false; }

        /** Overridden - see RenderTarget.
        */
        void getCustomAttribute(const String& name, void* pData);

        /** Overridden - see RenderTarget.
        */
        void writeContentsToFile(const String& filename);
		// DirectDraw Methods
		void createDDSurfaces(void);
		void releaseDDSurfaces(void);
		void restoreDDSurfaces(void);
		void createDepthBuffer(void);

    protected:
		HINSTANCE mInstance;			// Process instance
		DDDriver *mDriver;			// D3D9 driver
        // Win32-specific data members
        bool mIsUsingDirectDraw;
        HWND mHWnd;                    // Win32 Window handle
        HWND mParentHWnd;
		bool mIsExternal;			// window not created by Ogre
        bool mReady;                // Is ready i.e. available for update
        bool mClosed;

        RECT rcBlitDest;            // Target for blit operation
                                    // == window client area


        static LRESULT CALLBACK WndProc( 
            HWND hWnd, 
            UINT uMsg, 
            WPARAM wParam, 
            LPARAM lParam);

        // ---------------------------------------------------------
        // DirectX-specific
        // ---------------------------------------------------------

        // Pointer to DDDriver encapsulating DirectDraw driver (if DirectX in use)
        DDDriver* mlpDDDriver;

        // Pointers to DX surfaces
        LPDIRECTDRAWSURFACE7 mlpDDSFront; // Pointer to primary (front) buffer surface
        LPDIRECTDRAWSURFACE7 mlpDDSBack; // Pointer to back buffer

        // Pointer to the 3D Device specific for this window
        LPDIRECT3DDEVICE7 mlpD3DDevice;


        // Method for dealing with resize / move & 3d library
        void windowMovedOrResized(void);
        
        // ---------------------------------------------------------
        // OpenGL-specific details
        // ---------------------------------------------------------

        // TODO
    };
}
#endif
