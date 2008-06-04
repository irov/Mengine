
#include "D3D7RenderWindow.h"

//#include "OgreLogManager.h"
//#include "OgreViewport.h"
//#include "OgreException.h"
//#include "OgreRoot.h"
//#include "OgreRenderSystem.h"
#include "D3D7TextureManager.h"
//#include "OgreBitwise.h"
//#include "OgreImageCodec.h"
//#include "OgreStringConverter.h"
//#include "OgreWindowEventUtilities.h"

namespace Menge
{
    // Window procedure callback
    // This is a static member, so applies to all windows but we store the 
    // D3D7RenderWindow instance in the window data GetWindowLong/SetWindowLong.
    LRESULT D3D7RenderWindow::WndProc( 
        HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
    {
        LPCREATESTRUCT lpcs;
        D3D7RenderWindow* win;

        // look up window instance
        if (uMsg != WM_CREATE)
        {
            // Get window pointer
            win = (D3D7RenderWindow*)GetWindowLong(hWnd, 0);
        }

        switch( uMsg )
        {
            case WM_ACTIVATE:
                if( LOWORD( wParam ) == WA_INACTIVE )
                    win->mActive = false;
                else
                    win->mActive = true;
                break;

            case WM_CREATE:
                // Log the new window
                // Get CREATESTRUCT
                lpcs = (LPCREATESTRUCT)lParam;
                win = (D3D7RenderWindow*)(lpcs->lpCreateParams);
                // Store pointer in window user data area
                SetWindowLong(hWnd, 0, (long)win);
                win->mActive = true;

                return 0;

            case WM_KEYDOWN:
                // TEMPORARY CODE
                // TODO - queue up keydown / keyup events with
                //  window name and timestamp to be processed
                //  by main loop

                // ESCAPE closes window
                //if (wParam == VK_ESCAPE)
                //{
                //    win->mClosed = true;
                //    return 0L;
                //}
                break;

            case WM_PAINT:
                // If we get WM_PAINT messages, it usually means our window was
                // covered up, so we need to refresh it by re-showing the contents
                // of the current frame.
                if (win->mActive && win->mReady)
                    win->update();
                break;

			case WM_ENTERSIZEMOVE:
				// Prevent rendering while moving / sizing
				win->mReady = false;
				break;

			case WM_EXITSIZEMOVE:
				win->windowMovedOrResized();
				win->mReady = true;
				break;

            case WM_MOVE:
            case WM_SIZE:
                // Check to see if we are losing or gaining our window. Set the
                // active flag to match.
                if( SIZE_MAXHIDE==wParam || SIZE_MINIMIZED==wParam )
                    win->mActive = false;
                else
                {
                    win->mActive = true;
                    if (win->mReady)
                        win->windowMovedOrResized();
                }

                break;

            case WM_GETMINMAXINFO:
                // Prevent the window from going smaller than some minimum size
                ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 100;
                ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 100;
                break;

            case WM_CLOSE:
                DestroyWindow( win->mHWnd );
                win->mClosed = true;
                return 0;

            /*
            case WM_DESTROY:
                g_DIMSystem->CleanupRenderer();
                PostQuitMessage(0);
                return 0L;
            */
        }



        return DefWindowProc( hWnd, uMsg, wParam, lParam );
    }

    // -------------------------------------------
    // D3D7RenderWindow Implementation
    // -------------------------------------------
	D3D7RenderWindow::D3D7RenderWindow(HINSTANCE instance, DDDriver *driver):
		mInstance(instance),
		mDriver(driver)
    {
        mIsUsingDirectDraw = false;
        mIsFullScreen = false;
        mlpDDDriver = 0;
        mHWnd = 0;
        mActive = false;
        mReady = false;
        mClosed = false;
    }

    D3D7RenderWindow::~D3D7RenderWindow()
    {
    }



	void D3D7RenderWindow::create(const String& name, unsigned int width, unsigned int height,
	            bool fullScreen, const NameValuePairList *miscParams)
    {

        HWND parentHWnd = 0;
		HWND externalHandle = 0;
        HINSTANCE hInst = mInstance;
        DDDriver* drv = mDriver;
//        long tempPtr;
		bool vsync = false;
		unsigned int displayFrequency = 0;
		String title = name;
		unsigned int colourDepth = 32;
		unsigned int left = 0; // Defaults to screen center
		unsigned int top = 0; // Defaults to screen center
		bool depthBuffer = true;
		String border = "";
		bool outerSize = false;

		if(miscParams)
		{
			// Get variable-length params
			NameValuePairList::const_iterator opt;
			// left (x)
			opt = miscParams->find("left");
			if(opt != miscParams->end())
				left = StringConverter::parseUnsignedInt(opt->second);
			// top (y)
			opt = miscParams->find("top");
			if(opt != miscParams->end())
				top = StringConverter::parseUnsignedInt(opt->second);
			// Window title
			opt = miscParams->find("title");
			if(opt != miscParams->end())
				title = opt->second;
			// parentWindowHandle -> parentHWnd
			opt = miscParams->find("parentWindowHandle");
			if(opt != miscParams->end()) 
				parentHWnd = (HWND)StringConverter::parseUnsignedInt(opt->second);
			// externalWindowHandle		-> externalHandle
			opt = miscParams->find("externalWindowHandle");
			if(opt != miscParams->end())
				externalHandle = (HWND)StringConverter::parseUnsignedInt(opt->second);
			// vsync	[parseBool]
			opt = miscParams->find("vsync");
			if(opt != miscParams->end())
				vsync = StringConverter::parseBool(opt->second);
			// displayFrequency
			opt = miscParams->find("displayFrequency");
			if(opt != miscParams->end())
				displayFrequency = StringConverter::parseUnsignedInt(opt->second);
			// colourDepth
			opt = miscParams->find("colourDepth");
			if(opt != miscParams->end())
				colourDepth = StringConverter::parseUnsignedInt(opt->second);
			// depthBuffer [parseBool]
			opt = miscParams->find("depthBuffer");
			if(opt != miscParams->end())
				depthBuffer = StringConverter::parseBool(opt->second);
			// window border style
			opt = miscParams->find("border");
			if(opt != miscParams->end())
				border = opt->second;
			// set outer dimensions?
			opt = miscParams->find("outerDimensions");
			if(opt != miscParams->end())
				outerSize = StringConverter::parseBool(opt->second);

		}


        // Destroy current window if any
        if (mHWnd)
        {
            destroy();
        }

		mParentHWnd = parentHWnd;

		if (!externalHandle)
		{
			DWORD dwStyle = WS_VISIBLE | WS_CLIPCHILDREN;
			RECT rc;

			mWidth = width;
			mHeight = height;
			mTop = top;
			mLeft = left;

			if (!fullScreen)
			{
				if (parentHWnd)
				{
					dwStyle |= WS_CHILD;
				}
				else
				{
					if (border == "none")
						dwStyle |= WS_POPUP;
					else if (border == "fixed")
						dwStyle |= WS_OVERLAPPED | WS_BORDER | WS_CAPTION |
						WS_SYSMENU | WS_MINIMIZEBOX;
					else
						dwStyle |= WS_OVERLAPPEDWINDOW;
				}

				if (!outerSize)
				{
					// Calculate window dimensions required
					// to get the requested client area
					SetRect(&rc, 0, 0, mWidth, mHeight);
					AdjustWindowRect(&rc, dwStyle, false);
					mWidth = rc.right - rc.left;
					mHeight = rc.bottom - rc.top;

					// Clamp width and height to the desktop dimensions
					int screenw = GetSystemMetrics(SM_CXSCREEN);
					int screenh = GetSystemMetrics(SM_CYSCREEN);
					if ((int)mWidth > screenw)
						mWidth = screenw;
					if ((int)mHeight > screenh)
						mHeight = screenh;
					if (mLeft < 0)
						mLeft = (screenw - mWidth) / 2;
					if (mTop < 0)
						mTop = (screenh - mHeight) / 2;
				}
			}
			else
			{
				dwStyle |= WS_POPUP;
				mTop = mLeft = 0;
			}

			// Register the window class
			// NB allow 4 bytes of window data for D3D9RenderWindow pointer
			WNDCLASS wc = { 0, WindowEventUtilities::_WndProc, 0, 0, hInst,
				LoadIcon(0, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
				(HBRUSH)GetStockObject(BLACK_BRUSH), 0, "OgreD3D7Wnd" };
			RegisterClass(&wc);

			// Create our main window
			// Pass pointer to self
			mIsExternal = false;
			mHWnd = CreateWindow("OgreD3D7Wnd", title.c_str(), dwStyle,
				mLeft, mTop, mWidth, mHeight, parentHWnd, 0, hInst, this);

			WindowEventUtilities::_addRenderWindow(this);
		}
		else
		{
			mHWnd = externalHandle;
			mIsExternal = true;
		}

		RECT rc;
		// top and left represent outer window coordinates
		GetWindowRect(mHWnd, &rc);
		mTop = rc.top;
		mLeft = rc.left;
		// width and height represent interior drawable area
		GetClientRect(mHWnd, &rc);
		mWidth = rc.right;
		mHeight = rc.bottom;

		mName = name;
		mIsDepthBuffered = depthBuffer;
		mIsFullScreen = fullScreen;
		mColourDepth = colourDepth;

		StringUtil::StrStreamType str;
		str << "D3D7 : Created D3D7 Rendering Window '"
			<< mName << "' : " << mWidth << "x" << mHeight 
			<< ", " << mColourDepth << "bpp";
		LogManager::getSingleton().logMessage(
			LML_NORMAL, str.str());

        // Set up DirectDraw if appropriate
        // NB devices & surfaces set up for root window only
        if (drv && mParentHWnd == NULL)
        {
            // DD Driver object passed
            mlpDDDriver = drv;
            mIsUsingDirectDraw = true;

            createDDSurfaces();
            // Get best device based on render bit depth
            D3DDevice* dev = mlpDDDriver->get3DDeviceList()->getBest(mColourDepth);

            // create D3D device for this window
            mlpD3DDevice = dev->createDevice(mlpDDSBack);
            createDepthBuffer();
        }
        else
            mIsUsingDirectDraw = false;

        mReady = true;
		mActive = true;
		mClosed = false;

		windowMovedOrResized();
    }

    void D3D7RenderWindow::destroy(void)
    {
        if (mIsUsingDirectDraw)
            releaseDDSurfaces();

        DestroyWindow(mHWnd);

    }

    bool D3D7RenderWindow::isClosed(void) const
    {
        return mClosed;
    }

    void D3D7RenderWindow::reposition(int left, int top)
    {

    }

    void D3D7RenderWindow::resize(unsigned int width, unsigned int height)
    {
        mWidth = width;
        mHeight = height;

        // Notify viewports of resize
        ViewportList::iterator it = mViewportList.begin();
        while (it != mViewportList.end())
            (*it++).second->_updateDimensions();

        // TODO - resize window

    }

    void D3D7RenderWindow::swapBuffers(bool waitForVSync)
    {
		if (!mlpDDSFront)
			return;

        HRESULT hr;
        DWORD flags;
        if (mIsUsingDirectDraw)
        {
            if (mIsFullScreen)
            {
                // Use flipping chain
                if (waitForVSync)
                {
                    flags = DDFLIP_WAIT;
                }
                else
                {
                    flags = DDFLIP_WAIT | DDFLIP_NOVSYNC;
                }

                hr = mlpDDSFront->Flip(NULL,flags);
            }
            else
            {
                // Ordinary Blit
                RECT srcRect;
                srcRect.left = 0;
                srcRect.top = 0;
                srcRect.right = rcBlitDest.right - rcBlitDest.left;
                srcRect.bottom = rcBlitDest.bottom - rcBlitDest.top;
                hr = mlpDDSFront->Blt(&rcBlitDest,mlpDDSBack,&srcRect,DDBLT_WAIT,NULL);
            }

            if (hr == DDERR_SURFACELOST)
            {
                // Restore surfaces
                //restoreDDSurfaces();
            }
            else if (FAILED(hr))
            {
                 OGRE_EXCEPT( 
                     Exception::ERR_RENDERINGAPI_ERROR,
                     "Error flipping surfaces", 
                     "D3D7RenderWindow::swapBuffers" );
            }
        }

    }

    HWND D3D7RenderWindow::getWindowHandle(void) const
    {
        return mHWnd;
    }

    HWND D3D7RenderWindow::getParentWindowHandle(void) const
    {
        return mParentHWnd;
    }

    bool D3D7RenderWindow::isUsingDirectDraw(void) const
    {
        return mIsUsingDirectDraw;
    }

    // -------------------------------------------------------
    //   DirectDraw specific methods
    // -------------------------------------------------------
    DDDriver* D3D7RenderWindow::getDirectDrawDriver(void)
    {
        return mlpDDDriver;
    }

    LPDIRECTDRAWSURFACE7 D3D7RenderWindow::getDDFrontBuffer(void)
    {
        return mlpDDSFront;

    }

    LPDIRECTDRAWSURFACE7 D3D7RenderWindow::getDDBackBuffer(void)
    {
        return mlpDDSBack;
    }
    LPDIRECT3DDEVICE7 D3D7RenderWindow::getD3DDevice(void)
    {
        return mlpD3DDevice;
    }

    void D3D7RenderWindow::createDDSurfaces(void)
    {
        // Use DirectDraw wrapper object to create surfaces
        if( !mlpDDDriver )
        {
            OGRE_EXCEPT(
            Exception::ERR_INVALIDPARAMS, 
            "Cannot create surfaces because of no valid DirectDraw object",
            "D3D7RenderWindow::createDDSurfaces" );
        }

        if( mIsFullScreen )
        {
            mlpDDDriver->createWindowSurfaces(
                mHWnd, 
                mWidth, mHeight, mColourDepth,
                true, &mlpDDSFront, &mlpDDSBack);
        }
        else
        {
            // Windowed mode - need to use client rect for surface dimensions
            // I.e. we need to ignore menu bars, borders, title bar etc
            RECT rcClient;
            unsigned int cWidth, cHeight;

            GetClientRect( mHWnd, &rcClient );
            ClientToScreen( mHWnd, (POINT*)&rcClient.left );
            ClientToScreen( mHWnd, (POINT*)&rcClient.right );

            cWidth  = rcClient.right - rcClient.left;
            cHeight = rcClient.bottom - rcClient.top;

            // Create surfaces (AND clipper)
            mlpDDDriver->createWindowSurfaces(
                mHWnd, 
                cWidth, cHeight, 0,
                false, &mlpDDSFront, &mlpDDSBack);

            // Update own dimensions since target width / height is this
            mWidth = cWidth;
            mHeight = cHeight;
        }
    }

    void D3D7RenderWindow::createDepthBuffer(void)
    {
        // Get best device based on render bit depth
        D3DDevice* dev = mlpDDDriver->get3DDeviceList()->getBest(mColourDepth);
        if (mIsDepthBuffered && dev->NeedsZBuffer())
        {
            // Create Z buffer
            dev->createDepthBuffer(mlpDDSBack);
            // Don't set z buffer params here, leave to material
        }
    }

    void D3D7RenderWindow::releaseDDSurfaces(void)
    {
        // Release Z-buffer
        HRESULT hr;
        DDSCAPS2 ddscaps;

        ZeroMemory(&ddscaps, sizeof(DDSCAPS2));
        ddscaps.dwCaps = DDSCAPS_ZBUFFER;

		if (!mlpDDSBack->IsLost())
		{
			LPDIRECTDRAWSURFACE7 zBufSurface;

			hr = mlpDDSBack->GetAttachedSurface( &ddscaps, &zBufSurface );

			// Release twice as this method has increased
			zBufSurface->Release();
			zBufSurface->Release();
		}

        // Release std buffers
        mlpDDSBack->Release();
        mlpDDSFront->Release();

        mlpDDSBack = mlpDDSFront = 0;
    }

    void D3D7RenderWindow::restoreDDSurfaces(void)
    {
        HRESULT hr;

        if( mlpDDSFront->IsLost() )
        {
            hr = mlpDDSFront->Restore();

            if( FAILED( hr ) )
			{
				if (hr == DDERR_WRONGMODE)
				{
					// Fullscreen exclusive mode problem
					// Need to release & recreate
					releaseDDSurfaces();
					createDDSurfaces();
					createDepthBuffer();
					return;
				}
				else
				{
					char szBuffer[512];
					D3DXGetErrorString( hr, 512, szBuffer );
					OGRE_EXCEPT( 
						Exception::ERR_INTERNAL_ERROR, 
						"Error restoring lost primary surface." + String(szBuffer), 
						"D3D7RenderWindow - restoreDDSurfaces" );
				}
			}
        }

        if( mlpDDSBack->IsLost() )
        {
            hr = mlpDDSBack->Restore();

            if( FAILED( hr ) )
			{
				char szBuffer[512];
				D3DXGetErrorString( hr, 512, szBuffer );
                OGRE_EXCEPT( 
                    Exception::ERR_INTERNAL_ERROR, 
                    "Error restoring lost back buffer surface." + String(szBuffer), 
                    "D3D7RenderWindow - restoreDDSurfaces" );
			}
        }
    }

    void D3D7RenderWindow::windowMovedOrResized(void)
    {
        // If windowed mode, check window size & position
        RECT rcCheck;

        if( mIsFullScreen )
            return; // Nothing to check

        GetClientRect( mHWnd, &rcCheck );
        ClientToScreen( mHWnd, (POINT*)&rcCheck.left );
        ClientToScreen( mHWnd, (POINT*)&rcCheck.right );

		if ((rcCheck.right - rcCheck.left) == 0 ||
			(rcCheck.bottom - rcCheck.top) == 0)
		{
			return;
		}

        // Has the window resized? If so, we need to recreate surfaces
        if( ( rcCheck.right - rcCheck.left != 
                rcBlitDest.right - rcBlitDest.left ) ||
            ( rcCheck.bottom - rcCheck.top !=
                rcBlitDest.bottom - rcBlitDest.top ) )
        {
            // The window has changed size - DD surfaces must be recreated
            releaseDDSurfaces();
            createDDSurfaces();
            createDepthBuffer();

            // Update Viewport Sizes
            
            for( 
                ViewportList::iterator it = mViewportList.begin();
                it != mViewportList.end(); 
                ++it )
            {
                it->second->_updateDimensions();
            }

            rcBlitDest = rcCheck;
        }
        else if( 
            rcCheck.left != rcBlitDest.left ||
            rcCheck.top  != rcBlitDest.top )
        {
            // Window has only moved
            // Just alter the blit location
            rcBlitDest = rcCheck;
        }
    }

    //-----------------------------------------------------------------------
    void D3D7RenderWindow::getCustomAttribute( const String& name, void* pData )
    {
        // Valid attributes and their equivalent native functions:
        // D3DDEVICE            : getD3DDeviceDriver
        // DDBACKBUFFER         : getDDBackBuffer
        // DDFRONTBUFFER        : getDDFrontBuffer
        // HWND                 : getWindowHandle

        if( name == "D3DDEVICE" )
        {
            LPDIRECT3DDEVICE7 *pDev = (LPDIRECT3DDEVICE7*)pData;

            *pDev = getD3DDevice();
            return;
        }
        else if( name == "DDBACKBUFFER" )
        {
            LPDIRECTDRAWSURFACE7 *pSurf = (LPDIRECTDRAWSURFACE7*)pData;

            *pSurf = getDDBackBuffer();
            return;
        }
        else if( name == "DDFRONTBUFFER" )
        {
            LPDIRECTDRAWSURFACE7 *pSurf = (LPDIRECTDRAWSURFACE7*)pData;

            *pSurf = getDDFrontBuffer();
            return;
        }
        else if( name == "HWND" )
        {
            HWND *pHwnd = (HWND*)pData;

            *pHwnd = getWindowHandle();
            return;
        }
        else if( name == "isTexture" )
        {
            bool *b = reinterpret_cast< bool * >( pData );
            *b = false;

            return;
        }
    }


    void D3D7RenderWindow::writeContentsToFile(const String& filename)
    {
         HRESULT hr;
         LPDIRECTDRAWSURFACE7 pTempSurf;
         DDSURFACEDESC2 desc;
         RECT srcRect;
 
         // Cannot lock surface direct, so create temp surface and blit
         memset(&desc, 0, sizeof (DDSURFACEDESC2));
		 desc.dwSize = sizeof(DDSURFACEDESC2);
         if (FAILED(hr = mlpDDSBack->GetSurfaceDesc(&desc)))
         {
	         OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error getting description of back buffer!", 
                 "D3D7RenderWindow::writeContentsToFile");
         }

         desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
         desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;

		 if (FAILED(hr = mlpDDDriver->directDraw()->CreateSurface(&desc, &pTempSurf, NULL)))
		 {
             OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Error creating temporary surface!", 
                 "D3D7RenderWindow::writeContentsToFile");
		 }

         if (mIsFullScreen)
		 {
         	pTempSurf->Blt(NULL, mlpDDSFront, NULL, NULL, NULL);
		 }
         else
         {
         	GetWindowRect(mHWnd, &srcRect);
            srcRect.left += GetSystemMetrics(SM_CXSIZEFRAME);
            srcRect.top += GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYCAPTION);
            srcRect.right = srcRect.left + desc.dwWidth;
            srcRect.bottom = srcRect.top + desc.dwHeight;

            pTempSurf->Blt(NULL, mlpDDSFront, &srcRect, NULL, NULL);
         }

		 if (FAILED(hr = pTempSurf->Lock(NULL, &desc,  
             DDLOCK_WAIT | DDLOCK_READONLY, NULL)))
         {
             OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Cannot lock surface!", 
                 "D3D7RenderWindow::writeContentsToFile");
         }
 
 
         ImageCodec::ImageData *imgData = new ImageCodec::ImageData();
         imgData->width = desc.dwWidth;
         imgData->height = desc.dwHeight;
         imgData->format = PF_BYTE_RGB;
 
         // Allocate contiguous buffer (surfaces aren't necessarily contiguous)
         uchar* pBuffer = new uchar[desc.dwWidth * desc.dwHeight * 3];
 
         uint x, y;
         uchar *pData, *pDest;
 
         pData = (uchar*)desc.lpSurface;
         pDest = pBuffer;
         for (y = 0; y < desc.dwHeight; ++y)
         {
             uchar *pRow = pData;
 
             for (x = 0; x < desc.dwWidth; ++x)
             {
				 if (desc.ddpfPixelFormat.dwRGBBitCount == 16)
				 {
                     WORD val;
 
                     val = *((WORD*)pRow);
					 pRow += 2;

					 *pDest++ = Bitwise::convertBitPattern((WORD)val, (WORD)0xF800, (BYTE)0xFF);
					 *pDest++ = Bitwise::convertBitPattern((WORD)val, (WORD)0x07E0, (BYTE)0xFF);
					 *pDest++ = Bitwise::convertBitPattern((WORD)val, (WORD)0x001F, (BYTE)0xFF);
				 }
				 else
				 {
					 // Actual format is BRGA for some reason
                     *pDest++ = pRow[2]; // R
                     *pDest++ = pRow[1]; // G
                     *pDest++ = pRow[0]; // B

                     pRow += 3; // skip alpha / dummy

					 if (desc.ddpfPixelFormat.dwRGBBitCount == 32)
					 {
						 ++pRow; // Skip alpha
					 }
                 }
 
                 
             }
             // increase by one line
             pData += desc.lPitch;
 
         }
 
 
         // Wrap buffer in a chunk
         MemoryDataStreamPtr stream(new MemoryDataStream(pBuffer, desc.dwWidth * desc.dwHeight * 3, false));
 
         // Get codec 
         size_t pos = filename.find_last_of(".");
         String extension;
 	    if( pos == String::npos )
             OGRE_EXCEPT(
 		    Exception::ERR_INVALIDPARAMS, 
 		    "Unable to determine image type for '" + filename + "' - invalid extension.",
             "D3D8RenderWindow::writeContentsToFile" );
 
         while( pos != filename.length() - 1 )
             extension += filename[++pos];
 
         // Get the codec
         Codec * pCodec = Codec::getCodec(extension);
 
         // Write out
         pCodec->codeToFile(stream, filename, Codec::CodecDataPtr(imgData));
 
         delete [] pBuffer;
		 pTempSurf->Release();
 
     }

}

