#	pragma once

// Precompiler options
#include "D3D7Prerequisites.h"

// Declaration
//#include "OgreString.h"

namespace Menge
{
    class D3DDeviceList;
    class DDVideoModeList;
    class DDVideoMode;
    class D3DDevice;


    // Encapsulates a DirectDraw driver. Utility class for D3DRenderSystem.
    class _MengeD3DExport DDDriver
    {
    public:
        // Constructors
        DDDriver(); // Default
        DDDriver( const DDDriver& _ob ); // Copy
        DDDriver( GUID FAR *lpGuid,    // Create from enum
                                LPSTR lpDriverDescription,
                                LPSTR lpDriverName);
        // Destructor
        ~DDDriver();

		// Creates DirectDraw surfaces appropriate for a render window.

			//@remarks
			//	This method creates double-buffered surfaces for a window. Note that in this case the
			//	metrics supplied (width, height) must be of the CLIENT area of a window only
			//	if fullScreen = false; i.e. the metrics must be ignoring title bar, menu bar etc.

			//@param hWnd 
			//	Window handle to the window for which the surfaces are created.
			//@param width 
			//	The width of the surfaces required (see note above).
			//@param height 
			//	The height of the surfaces required (see note above).
			//@param colourDepth 
			//	The colour depth in bits per pixel. Only relevant if fullScreen = true.
			//@param fullScreen 
			//	Specify true to make these surfaces full screen exclusive.
			//@param front 
			//	Pointer to a pointer which will be filled with the interface to the front buffer on return.
			//@param back 
			//	Pointer to a pointer which will be filled with the interface to the back buffer on return.
			//	Only relevant if fullScreen = false.

        void createWindowSurfaces( HWND _hWnd, unsigned int _width, unsigned int _height, unsigned int _colourDepth, bool _fullScreen,
            LPDIRECTDRAWSURFACE7* _front, LPDIRECTDRAWSURFACE7* _back );

        void cleanup();
        void checkWindow();

        // Overloaded operator =
        DDDriver operator=(const DDDriver& _orig);

        // Information accessors
        String driverName() const;
        String driverDescription() const;
        LPDIRECTDRAW7 directDraw(); // Gets lpDD7 (instantiates if required)

        D3DDeviceList* get3DDeviceList();
        DDVideoModeList* getVideoModeList();
        DDVideoMode* getActiveVideoMode();

        // Retrieval of other objects
        D3DDevice* get3DDevice();

        void outputText( int _x, int _y, char* _text );
        void flipBuffers();
        bool runningFullScreen() const;
        RECT viewportRect() const;

        // Capabilities
        bool has3DAcceleration() const;
        bool canRenderWindowed() const;

        // Generalised info
        void getDisplayDetails( unsigned int& _width, unsigned int& _height, unsigned int& _colourDepth );
	private:

		String m_driverName;
		String m_driverDesc;
		GUID  m_guid; // Note - copy of GUID, not pointer to
		bool  m_primaryDisplay;

		LPDIRECTDRAW7 lpDD7; // pointer to IDirectDraw7 interface
		LPDIRECT3D7   lpD3D; // pointer to IDirect3D7 interface (if required)
		D3DDeviceList* m_deviceList; // List of Direct3D devices
		DDVideoModeList* m_videoModeList; // List of video modes
		D3DDevice* m_active3DDevice;
		DDVideoMode* m_activeVideoMode;
		HWND m_activeHWnd;

		DDCAPS m_SWCaps; // Software capabilities
		DDCAPS m_HWCaps; // Hardware capabilities
		void logCaps() const;

		LPDIRECTDRAWSURFACE7 lpDDSPrimary; // Pointer to primary surface
		LPDIRECTDRAWSURFACE7 lpDDSBack; // Pointer to back buffer

		bool m_runningFullScreen;
		bool m_using3DMode;

		RECT m_viewport; //Only used for windowed mode
		LPDIRECTDRAWCLIPPER lpDDClipper;

		// Private accessor functions
		void _restoreSurfaces();
    };
}