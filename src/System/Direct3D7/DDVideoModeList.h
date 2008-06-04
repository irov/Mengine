#	pragma once 

// Precompiler options
#	include "D3D7Prerequisites.h"

#	include <vector>

namespace Menge
{
    class DDVideoMode;
    
	// Encapsulates enumeration of DirectDraw video modes. Utility class for D3DRenderSystem.
    class DDVideoModeList
    {
	public:
		DDVideoModeList( LPDIRECTDRAW7 _lpDD7 ); // Only allowed Constructor
		~DDVideoModeList(); // Destructor
		BOOL enumerate(); //Forces an enumeration / re-enumeration

		// List accessors
		DDVideoMode* item( int _index );
		unsigned int count();
		DDVideoMode* getClosestMatch( int _width, int _height, int _colourDepth );



		// Non-member callbacks
		friend static HRESULT CALLBACK DDEnumModesCallback(
			LPDDSURFACEDESC2 lpDDSD2,
			LPVOID lpContext);


    private:
        // Pointer to DDD interface
        LPDIRECTDRAW7 lpDD7;

        // STL list of drivers (Vector used to allow random access)
        std::vector<DDVideoMode> m_modeList;

        void AddMode( LPDDSURFACEDESC2 lpDDSurfaceDesc );


    };
} // namespace Menge
