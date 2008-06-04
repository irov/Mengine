
#include "D3D7RenderSystem.h"
#include "DDDriver.h"
#include "DDVideoModeList.h"
#include "DDVideoMode.h"
#include "D3D7DeviceList.h"
#include "D3D7Device.h"
//#include "OgreLogManager.h"
//#include "OgreException.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
    DDDriver DDDriver::operator=( const DDDriver& _orig )
    {
        m_guid = _orig.mGuid;
        //mDriverDesc = new char(strlen(orig.mDriverDesc) + 1);
        //strcpy(mDriverDesc, orig.mDriverDesc);
        m_driverDesc = _orig.mDriverDesc;
        //mDriverName = new char(strlen(orig.mDriverName)+1);
        //strcpy(mDriverName, orig.mDriverName);
        m_driverName = _orig.mDriverName;
        m_primaryDisplay = _orig.mPrimaryDisplay;
        active3DDevice = _orig.active3DDevice;
        activeVideoMode = _orig.activeVideoMode;
        lpD3D = _orig.lpD3D;
        lpDD7 = _orig.lpDD7;
        lpDDClipper = _orig.lpDDClipper;
        lpDDSBack = _orig.lpDDSBack;
        lpDDSPrimary = _orig.lpDDSPrimary;
        m_SWCaps = _orig.mSWCaps;
        m_HWCaps = _orig.mHWCaps;
        m_deviceList = _orig.mDeviceList;
        m_videoModeList = _orig.mVideoModeList;


        return *this;
    }
	//////////////////////////////////////////////////////////////////////////
    DDDriver::DDDriver()
    {
        // Init pointers
        active3DDevice = NULL;
        activeHWnd = 0;
        activeVideoMode = NULL;
        lpD3D = NULL;
        lpDD7 = NULL;
        lpDDClipper = NULL;
        lpDDSBack = NULL;
        lpDDSPrimary = NULL;
        mDeviceList = NULL;
        mVideoModeList = NULL;


    }
	//////////////////////////////////////////////////////////////////////////
    DDDriver::~DDDriver()
    {
        // Delete related system objects
        if (mDeviceList)
            delete mDeviceList;
        if (mVideoModeList)
            delete mVideoModeList;

    }
	//////////////////////////////////////////////////////////////////////////
    DDDriver::DDDriver(const DDDriver &ob)
    {
        mGuid = ob.mGuid;
        //mDriverDesc = new char(strlen(ob.mDriverDesc) + 1);
        //strcpy(mDriverDesc, ob.mDriverDesc);
        mDriverDesc = ob.mDriverDesc;
        //mDriverName = new char(strlen(ob.mDriverName)+1);
        //strcpy(mDriverName, ob.mDriverName);
        mDriverName = ob.mDriverName;
        mPrimaryDisplay = ob.mPrimaryDisplay;
        active3DDevice = ob.active3DDevice;
        activeVideoMode = ob.activeVideoMode;
        lpD3D = ob.lpD3D;
        lpDD7 = ob.lpDD7;
        lpDDClipper = ob.lpDDClipper;
        lpDDSBack = ob.lpDDSBack;
        lpDDSPrimary = ob.lpDDSPrimary;
        mSWCaps = ob.mSWCaps;
        mHWCaps = ob.mHWCaps;
        mDeviceList = ob.mDeviceList;
        mVideoModeList = ob.mVideoModeList;
    }
	//////////////////////////////////////////////////////////////////////////
    DDDriver::DDDriver(GUID FAR *lpGuid,
                                LPSTR lpDriverDescription,
                                LPSTR lpDriverName)
    {
        HRESULT hr;

        // Init pointers
        active3DDevice = NULL;
        activeHWnd = 0;
        activeVideoMode = NULL;
        lpD3D = NULL;
        lpDD7 = NULL;
        lpDDClipper = NULL;
        lpDDSBack = NULL;
        lpDDSPrimary = NULL;
        mDeviceList = NULL;
        mVideoModeList = NULL;

        // Copy GUID, Description and Name
        // Deal with NULL (default display driver)
        if (lpGuid)
        {
            memcpy(&mGuid, lpGuid, sizeof(GUID));
            mPrimaryDisplay = false;
        }
        else
            mPrimaryDisplay = true;


        mDriverDesc = String(lpDriverDescription);
        //mDriverDesc = new char(strlen(lpDriverDescription)+1);
        //strcpy(mDriverDesc, lpDriverDescription);

        mDriverName = String(lpDriverName);
        //mDriverName = new char(strlen(lpDriverName)+1);
        //strcpy(mDriverName, lpDriverName);


        // Get capabilities
        mHWCaps.dwSize = sizeof(DDCAPS);
        mSWCaps.dwSize = sizeof(DDCAPS);

        hr = directDraw()->GetCaps(&mHWCaps, &mSWCaps);
        assert( SUCCEEDED(hr) && "DIM_DDDriver - directDraw() -> Cannot get direct draw driver capabilities." );
    }
	//////////////////////////////////////////////////////////////////////////
    void DDDriver::createWindowSurfaces( HWND _hWnd, unsigned int _width, unsigned int _height, unsigned int _colourDepth, bool _fullScreen,
            LPDIRECTDRAWSURFACE7* _front, LPDIRECTDRAWSURFACE7* _back )
    {
        char msg[150];
        DWORD dwFlags;
        DDSURFACEDESC2 ddsd;
        HRESULT hr;
        DDSCAPS2 ddscaps;
        LPDIRECTDRAWCLIPPER clip;

        LogManager::getSingleton().logMessage("Creating DirectDraw surfaces for window with dimensions:");

        if( _fullScreen )
		{
            sprintf(msg, "  FULLSCREEN w:%i h:%i bpp:%i", _width, _height, _colourDepth );
		}
        else
		{
            sprintf(msg, "  WINDOWED w:%i h:%i", _width, _height );
		}

        LogManager::getSingleton().logMessage(msg);

        // Set co-op level
        if( fullScreen )
        {
            dwFlags = DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE;
        }
        else
        {
            dwFlags = DDSCL_NORMAL;
        }

        // Also set FPU cooperative mode (optimisation)
        dwFlags = dwFlags | DDSCL_FPUSETUP;

        hr = directDraw()->SetCooperativeLevel( _hWnd, dwFlags );
        assert( SUCCEEDED(hr) && "DDDriver - createWindowSurfaces -> Error setting cooperative mode" );

        // Create surfaces
        // If we're running fullscreen, create a complex flipping
        // chain surface (implicit back buffer)
        // Otherwise we need separate back buffers

        if( _fullScreen )
        {
            // Set Video Mode
            hr = lpDD7->SetDisplayMode( _width, _height, _colourDepth, 0, 0 );
            assert( SUCCEEDED(hr) && "DDDriver - createWindowSurfaces -> Unable to set fullScreen display mode." );

            // Set up surfaces
            ZeroMemory( &ddsd, sizeof(DDSURFACEDESC2) );
            ddsd.dwSize = sizeof(DDSURFACEDESC2);
            ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
            ddsd.ddsCaps.dwCaps = DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE;

            // Create a single back buffer
            ddsd.dwBackBufferCount = 1;
            hr = directDraw()->CreateSurface(&ddsd,front,NULL);
            assert( SUCCEEDED(hr) && "DDDriver - createWindowSurfaces -> Error creating linked surface buffers" );

            // Get back buffer
            ZeroMemory(&ddscaps, sizeof(DDSCAPS2));
            ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
            hr = (*front)->GetAttachedSurface( &ddscaps, back );

            assert( SUCCEEDED(hr) && "DDDriver - createWindowSurfaces -> Error retrieving linked back buffer" );

            LogManager::getSingleton().logMessage("Successfully created full screen rendering surface / flipping chain.");
        }
        else
        {
            // Explicitly create front and back buffers, and create
            // a clipper object

            // Standard non-auto-flip primary
            ZeroMemory( &ddsd, sizeof(DDSURFACEDESC2) );
            ddsd.dwSize  = sizeof(DDSURFACEDESC2);
            ddsd.dwFlags = DDSD_CAPS;
            ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
            hr = directDraw()->CreateSurface(&ddsd,front,NULL);
            assert( SUCCEEDED(hr) && "DDDriver - createWindowSurfaces -> Error creating primary surface buffer" );

            // Create offscreen buffer
            ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
            ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;
            // Set the dimensions of the back buffer. Note that if our window changes
            // size, we need to destroy this surface and create a new one.
            ddsd.dwWidth  = width;
            ddsd.dwHeight = height;
            // Create the back buffer. The most likely reason for failure is running
            // out of video memory. (A more sophisticated app should handle this.)
            hr = directDraw()->CreateSurface( &ddsd, back, NULL );
            assert( SUCCEEDED( hr ) && "DDDriver - createWindowSurfaces -> Error creating back surface buffer" );

            LogManager::getSingleton().logMessage("Windowed mode rendering & display surfaces created.");

            // Create clipper
            hr = directDraw()->CreateClipper(0, &clip, NULL);
            assert( SUCCEEDED(hr) && "DDDriver - createWindowSurfaces -> Error creating clipper" );

            // Assign it to the window
            clip->SetHWnd(0,hWnd);
            // Assign it to primary surface
            (*front)->SetClipper(clip);
            clip->Release();

            LogManager::getSingleton().logMessage("Window clipper created.");

        }
    }
	//////////////////////////////////////////////////////////////////////////
    String DDDriver::driverName() const
    {
        return m_driverName;
    }
	//////////////////////////////////////////////////////////////////////////
    String DDDriver::driverDescription() const
    {
        return m_driverDesc;
    }
	//////////////////////////////////////////////////////////////////////////
    D3DDeviceList* DDDriver::get3DDeviceList()
    {
        if (!m_deviceList)
		{
            m_deviceList = new D3DDeviceList( lpD3D );
		}

        return m_deviceList;
    }
	//////////////////////////////////////////////////////////////////////////
    DDVideoModeList* DDDriver::getVideoModeList()
    {
        if( !m_videoModeList )
		{
            m_videoModeList = new DDVideoModeList(lpDD7);
		}

        return m_videoModeList;
    }
	//////////////////////////////////////////////////////////////////////////
    DDVideoMode* DDDriver::getActiveVideoMode()
    {
        return activeVideoMode;
    }
	//////////////////////////////////////////////////////////////////////////
    D3DDevice* DDDriver::get3DDevice()
    {
        return active3DDevice;
    }
	//////////////////////////////////////////////////////////////////////////
    LPDIRECTDRAW7 DDDriver::directDraw()
    {
        HRESULT rVal;

        if ( !lpDD7 )
        {
            // We need to create a direct draw object
            // Create with GUID for this driver
            if( m_primaryDisplay )
			{
                rVal = DirectDrawCreateEx( NULL, (VOID**)&lpDD7, IID_IDirectDraw7, NULL );
			}
            else
			{
                rVal = DirectDrawCreateEx( &mGuid, (VOID**)&lpDD7, IID_IDirectDraw7, NULL );
			}
            assert( ( rVal == DD_OK ) && "DIM_DDDriver - directDraw() -> Cannot create direct draw interface." );

            // Get Direct3D interface too
            lpDD7->QueryInterface( IID_IDirect3D7, (VOID**)&lpD3D );
        }

        return lpDD7;
    }
	//////////////////////////////////////////////////////////////////////////
    void DDDriver::cleanup()
    {
        // Tell 3D Device to clean itself up
        if( active3DDevice )
        {
            active3DDevice->Cleanup();
        }

        if( activeHWnd && lpDD7 )
		{
            lpDD7->SetCooperativeLevel( activeHWnd, DDSCL_NORMAL );
		}

        // Release DirectX system objects
        if ( lpDDSBack )
        {
            lpDDSBack->Release();
            lpDDSBack = NULL;
        }

        if( lpDDSPrimary )
        {
            lpDDSPrimary->Release();
            lpDDSPrimary = NULL;
        }

        if( lpD3D )
        {
            lpD3D->Release();
            lpD3D = NULL;
        }

        if( lpDD7 )
        {
            lpDD7->Release();
            lpDD7 = NULL;
        }
    }
	//////////////////////////////////////////////////////////////////////////
    void DDDriver::checkWindow()
    {
        // If windowed mode, check window size & position
        RECT rcCheck;

        if( RunningFullScreen() )
		{
            return;
		}

        GetClientRect( activeHWnd, &rcCheck );
        ClientToScreen( activeHWnd, (POINT*)&rcCheck.left );
        ClientToScreen( activeHWnd, (POINT*)&rcCheck.right );

        // Has the window resized? If so, we need to recreate surfaces
        if ((rcCheck.right - rcCheck.left !=
                rcViewport.right - rcViewport.left) ||
            (rcCheck.bottom - rcCheck.top !=
                rcViewport.bottom - rcViewport.top))
        {
            // The window has changed size
            //g_DIMSystem->CleanupRenderer();
            //g_DIMSystem->Reinitialise();
        }
        else if (rcCheck.left != rcViewport.left ||
            rcCheck.top != rcViewport.top)
        {
            // Window has only moved
            // Just alter the blit location
            rcViewport = rcCheck;
        }
    }
	//////////////////////////////////////////////////////////////////////////
    void DDDriver::flipBuffers(void)
    {
        HRESULT hr;
        if (runningFullScreen)
        {
            // Use flipping chain
            hr = lpDDSPrimary->Flip(NULL,DDFLIP_WAIT);
        }
        else
        {
            // Ordinary Blit
            RECT srcRect;
            srcRect.left = 0;
            srcRect.top = 0;
            srcRect.right = rcViewport.right - rcViewport.left;
            srcRect.bottom = rcViewport.bottom - rcViewport.top;
            hr = lpDDSPrimary->Blt(&rcViewport,lpDDSBack,&srcRect,DDBLT_WAIT,NULL);
        }

        if (hr == DDERR_SURFACELOST)
        {
            // Restore surfaces
            RestoreSurfaces();
        }
        else
        {
            assert( SUCCEEDED(hr) && "DDDriver::FlipBuffers -> Error flipping surfaces" );
        }
    }
	//////////////////////////////////////////////////////////////////////////
    void DDDriver::restoreSurfaces()
    {
        HRESULT hr;

        if ( lpDDSPrimary->IsLost() )
        {
            hr = lpDDSPrimary->Restore();
            assert( SUCCEEDED(hr) && "DDDriver - RestoreSurfaces -> Error restoring lost primary surface." );
        }

        if (lpDDSBack->IsLost())
        {
            hr = lpDDSBack->Restore();
            assert( SUCCEEDED(hr) && "DDDriver - RestoreSurfaces -> Error restoring lost back buffer surface." );
        }
    }
	//////////////////////////////////////////////////////////////////////////
    void DDDriver::outputText( int _x, int _y, char* _text )
    {
        HDC hDC;

        // Get a DC for the surface. Then, write out the buffer
        if( lpDDSBack )
        {
            if( SUCCEEDED( lpDDSBack->GetDC(&hDC) ) )
            {
                SetTextColor( hDC, RGB(255,255,0) );
                SetBkMode( hDC, TRANSPARENT );
                ExtTextOut( hDC, _x, _y, 0, NULL, text, lstrlen(_text), NULL );
                lpDDSBack->ReleaseDC(hDC);
            }
        }
    }
	//////////////////////////////////////////////////////////////////////////
    bool DDDriver::runningFullScreen() const
    {
        return runningFullScreen;
    }
	//////////////////////////////////////////////////////////////////////////
    RECT DDDriver::viewportRect() const
    {
        return rcViewport;
    }
	//////////////////////////////////////////////////////////////////////////
    bool DDDriver::canRenderWindowed() const
    {
        return (mHWCaps.dwCaps2 & DDCAPS2_CANRENDERWINDOWED) > 0;
    }
	//////////////////////////////////////////////////////////////////////////
    bool DDDriver::has3DAcceleration() const
    {
        return (mHWCaps.dwCaps & DDCAPS_3D);
    }
	//////////////////////////////////////////////////////////////////////////
    void DDDriver::getDisplayDetails( unsigned int& _width, unsigned int& _height, unsigned int& _colourDepth )
    {
        // Get details from primary surface
        // This works for both windowed and fullscreen modes

        DDSURFACEDESC2 ddsd;

        if (lpDDSPrimary)
        {
            ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
            ddsd.dwSize = sizeof(DDSURFACEDESC2);
            lpDDSPrimary->GetSurfaceDesc(&ddsd);

            _width = ddsd.dwWidth;
            _height = ddsd.dwHeight;
            _colourDepth = ddsd.ddpfPixelFormat.dwRGBBitCount;
        }
    }
	//////////////////////////////////////////////////////////////////////////
    void DDDriver::logCaps() const
    {
        // Sends capabilities of this driver to the log
        char msg[255];

        LogManager::getSingleton().logMessage("DirectDraw Driver Capabilities:");

        sprintf(msg, "  3D Acceleration: %i", Has3DAcceleration());
        LogManager::getSingleton().logMessage(msg);

        sprintf(msg, "  Render in a window: %i", CanRenderWindowed());
        LogManager::getSingleton().logMessage(msg);
    }
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge


