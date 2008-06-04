#	pragma once

#include "D3D7RenderSystem.h"
#include "DDVideoMode.h"

#	include <sstream>

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    DDVideoMode::DDVideoMode()
    {
        // Nothing
    }
	//////////////////////////////////////////////////////////////////////////
    DDVideoMode::DDVideoMode( const DDVideoMode& _ob )
    {
        m_width = _ob.m_width;
        m_height = _ob.m_height;
        m_colourDepth = _ob.m_colourDepth;
        m_DDSurfaceDesc = _ob.m_DDSurfaceDesc;
    }
	//////////////////////////////////////////////////////////////////////////
	DDVideoMode::DDVideoMode( LPDDSURFACEDESC2 lpDDSD2 )
    {
        // Copy surface desc
        memcpy( &m_DDSurfaceDesc, lpDDSD2, sizeof( DDSURFACEDESC2 ) );

        // Determine basic information
        m_width = m_DDSurfaceDesc.dwWidth;
        m_height = m_DDSurfaceDesc.dwHeight;
        m_colourDepth = m_DDSurfaceDesc.ddpfPixelFormat.dwRGBBitCount;
    }
	//////////////////////////////////////////////////////////////////////////
    DDVideoMode DDVideoMode::operator=( const DDVideoMode& _orig )
    {

        m_width = _orig.m_width;
        m_height = _orig.m_height;
        m_colourDepth = _orig.m_colourDepth;
        m_DDSurfaceDesc = _orig.m_DDSurfaceDesc;

        return *this;
    }
	//////////////////////////////////////////////////////////////////////////
    String DDVideoMode::Description()
    {
        /*char tmp[128];

        sprintf(tmp, "%d x %d @ %d-bit colour", m_width, m_height, m_colourDepth);
        return String(tmp);*/
		std::ostringstream out;
		out << m_width << " x " << m_height << " @ " << m_colourDepth << "-bit colour";
		return out.str();
    }
	//////////////////////////////////////////////////////////////////////////
}
