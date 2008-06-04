#	pragma once

// Precompiler options
#	include "Config/Typedef.h"
#include "D3D7Prerequisites.h"

//#include "OgreString.h"

namespace Menge
{
    //Encapsulates a DirectDraw video mode. Utility class for D3DRenderSystem.
    class DDVideoMode
    {
    public:
		// Constructors
		DDVideoMode(); // Default
		DDVideoMode( const DDVideoMode& _ob ); // Copy
		DDVideoMode( LPDDSURFACEDESC2 lpDDSD2 ); // enum

		// Overloaded operator =
		DDVideoMode operator=( const DDVideoMode& _orig );

		String Description();

		// Summary info
        int m_width;
        int m_height;
        int m_colourDepth;

    private:
        DDSURFACEDESC2 m_DDSurfaceDesc; // Complete info

    };
} // Namespace Menge
