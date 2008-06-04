
#include "D3D7RenderSystem.h"
#include "D3D7Device.h"
#include "DDDriver.h"
#include "DDVideoMode.h"
//#include "Light.h"
//#include "Camera.h"
//#include "LogManager.h"
//#include "Exception.h"
#	include <sstream>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static HRESULT CALLBACK EnumZBuffersCallback(DDPIXELFORMAT* pddpf,
            VOID* pFormats)
    {
        // Add to list of formats
        std::vector<DDPIXELFORMAT> *vec;
        vec = (std::vector<DDPIXELFORMAT>*)pFormats;

        if (pddpf->dwFlags & DDPF_ZBUFFER)
            vec->push_back(*pddpf);
        return D3DENUMRET_OK;

    }
	//////////////////////////////////////////////////////////////////////////
    D3DDevice D3DDevice::operator=(const D3DDevice &orig)
    {
        m_deviceName = orig.m_deviceName;
        m_deviceDescription = orig.m_deviceDescription;
        m_D3DDeviceDesc = orig.m_D3DDeviceDesc;
        m_isHardwareAccelerated = orig.m_isHardwareAccelerated;
        m_needsZBuffer = orig.m_needsZBuffer;


        return *this;
    }
	//////////////////////////////////////////////////////////////////////////
    D3DDevice::D3DDevice()
    {
        // Init pointers
        lpD3D = NULL;

    }
	//////////////////////////////////////////////////////////////////////////
    D3DDevice::D3DDevice(const D3DDevice &ob)
    {
        lpD3D = ob.lpD3D;
        m_viewport = ob.m_viewport;
        m_deviceDescription = ob.m_deviceDescription;
        m_deviceName = ob.m_deviceName;
        m_D3DDeviceDesc = ob.m_D3DDeviceDesc;
        m_isHardwareAccelerated = ob.m_isHardwareAccelerated;
        m_needsZBuffer = ob.m_needsZBuffer;

    }
	//////////////////////////////////////////////////////////////////////////
    D3DDevice::D3DDevice(LPDIRECT3D7 lpDirect3D, LPSTR lpDeviceDesc, LPSTR lpDeviceName,
                    LPD3DDEVICEDESC7 lpD3DDeviceDesc)
    {
        // Init pointers
        lpD3D = NULL;

        // Copy pointer to Direct3D7 interface
        lpD3D = lpDirect3D;

        // Copy Name and Description
        m_deviceDescription = lpDeviceDesc;
        m_deviceName = lpDeviceName;

        // Is this a hardware or emulation device?
        m_isHardwareAccelerated = ( 0 != (lpD3DDeviceDesc->dwDevCaps & D3DDEVCAPS_HWRASTERIZATION) );


        // Copy device description
        // No need to differentiate between SW and HW anymore
        memcpy(&m_D3DDeviceDesc, lpD3DDeviceDesc, sizeof(D3DDEVICEDESC7));

		std::ostringstream str;
        str << "Detected Direct3D Device " << lpDeviceDesc;
        LogManager::getSingleton().logMessage(str.str());
        logCaps();

        // Do we need a Z Buffer?
        m_needsZBuffer = !(m_D3DDeviceDesc.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_ZBUFFERLESSHSR);
        if (m_needsZBuffer)
            LogManager::getSingleton().logMessage("This device needs a Z-Buffer");
        else
            LogManager::getSingleton().logMessage("This device does not need a Z-Buffer");


    }
	//////////////////////////////////////////////////////////////////////////
    D3DDevice::~D3DDevice()
    {
    }
	//////////////////////////////////////////////////////////////////////////
    LPDIRECT3DDEVICE7 D3DDevice::createDevice( LPDIRECTDRAWSURFACE7 _renderTarget )
    {
        LPDIRECT3DDEVICE7 dev;
        HRESULT hr;

        hr = lpD3D->CreateDevice( m_D3DDeviceDesc.deviceGUID, _renderTarget, &dev );
        assert( SUCCEEDED(hr) && "D3DDevice::createDevice -> Error creating new D3D device." );

        return dev;
    }
	//////////////////////////////////////////////////////////////////////////
    LPDIRECT3D7 D3DDevice::getID3D()
    {
        return lpD3D;
    }
	//////////////////////////////////////////////////////////////////////////
    bool D3DDevice::isHardwareAccelerated() const
    {
        return m_isHardwareAccelerated;
    }
	//////////////////////////////////////////////////////////////////////////
    void D3DDevice::logCaps() const
    {
        // Sends capabilities of this driver to the log
        String msg;

        //LogManager::getSingleton().logMessage("Direct3D Device Capabilities:");

        msg << "  Hardware Accelerated: " << hardwareAccelerated();
        //LogManager::getSingleton().logMessage(msg);

        msg << "  Mipmapping: " << canMipmap();
        //LogManager::getSingleton().logMessage(msg);

        msg << "  Bilinear Filtering: " << canBilinearFilter());
        //LogManager::getSingleton().logMessage(msg);

        msg << "  Trilinear Filtering: " << canTrilinearFilter());
        //LogManager::getSingleton().logMessage(msg);

        msg << "  Hardware Transform & Light: " << canHWTransformAndLight();
        //LogManager::getSingleton().logMessage(msg);

        msg << "  Max rendering colour depth: " << renderBitDepth();
        //LogManager::getSingleton().logMessage(msg);

        msg << "  Max single-pass texture layers: " << maxSinglePassTextureLayers();
        //LogManager::getSingleton().logMessage(msg);

        msg << "  Pixel fog supported: " << ( m_D3DDeviceDesc.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_FOGTABLE ) );
        //LogManager::getSingleton().logMessage(msg);

        msg << "  Vertex fog supported: " << ( m_D3DDeviceDesc.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_FOGVERTEX) );
        //LogManager::getSingleton().logMessage(msg);
    }
	//////////////////////////////////////////////////////////////////////////
    void D3DDevice::cleanup()
    {
        // Release DirectX Objects

        lpD3D = NULL;
    }
	//////////////////////////////////////////////////////////////////////////
    String D3DDevice::deviceName() const
    {
        return m_deviceName;
    }
	//////////////////////////////////////////////////////////////////////////
    String D3DDevice::deviceDescription() const
    {
        return m_deviceDescription;
    }
	//////////////////////////////////////////////////////////////////////////
    void D3DDevice::createDepthBuffer( LPDIRECTDRAWSURFACE7 renderTarget )
    {
        DWORD bestDepth, bestStencil;
        DDSURFACEDESC2 ddsd, src_ddsd;
        LPDIRECTDRAW7 lpDD7;
        LPDIRECTDRAWSURFACE7 lpZBuffer;
        HRESULT hr;

        LogManager::getSingleton().logMessage("Direct3D - Creating Z-Buffer");

        // First check we NEED a depth buffer - e.g. PowerVR doesn't need one
        if (m_needsZBuffer)
        {
            // Get description from source surface
            ZeroMemory(&src_ddsd, sizeof(DDSURFACEDESC2));
            src_ddsd.dwSize = sizeof(DDSURFACEDESC2);
            renderTarget->GetSurfaceDesc(&src_ddsd);

            // Enumerate Depth Buffers
            m_depthBufferFormats.clear();
            lpD3D->EnumZBufferFormats(
				m_D3DDeviceDesc.deviceGUID, 
				EnumZBuffersCallback, 
				(LPVOID)&m_depthBufferFormats );

            // Choose the best one
            // NB make sure Z buffer is the same depth as colour buffer (GeForce TnL problem)
            // Also use best stencil if z depth is the same
            bestDepth = 0;
            bestStencil = 0;

            std::vector<DDPIXELFORMAT>::iterator it, best_it;
            for( 
                it = m_depthBufferFormats.begin();
                it != m_depthBufferFormats.end();
                ++it )
            {
                if( ( (*it).dwZBufferBitDepth > bestDepth || (*it).dwStencilBitDepth > bestStencil)
                    &&
                    (*it).dwZBufferBitDepth <= src_ddsd.ddpfPixelFormat.dwZBufferBitDepth )
                {
                    best_it = it;
                    bestDepth = (*it).dwZBufferBitDepth;
                    bestStencil = (*it).dwStencilBitDepth;
                }
            }

            // Setup the surface desc for the z-buffer.
            ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));

            ddsd.dwSize         = sizeof(DDSURFACEDESC2);
            ddsd.dwFlags        = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;

            ddsd.dwWidth        = src_ddsd.dwWidth;
            ddsd.dwHeight       = src_ddsd.dwHeight;

            ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER;

            memcpy( &ddsd.ddpfPixelFormat, &(*best_it), sizeof(DDPIXELFORMAT) );

            // Software devices require system-memory depth buffers.
            if( m_isHardwareAccelerated )
                ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
            else
                ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

            // Create the depth-buffer.
            renderTarget->GetDDInterface( (VOID**)&lpDD7 );
            lpDD7->Release();

            if( FAILED( hr = lpDD7->CreateSurface( &ddsd, &lpZBuffer, NULL ) ) )
                OGRE_EXCEPT(
                    Exception::ERR_RENDERINGAPI_ERROR, 
                    "Error creating depth buffer",
                    "D3DDevice::createDepthBuffer" );

            if( FAILED( hr = renderTarget->AddAttachedSurface(lpZBuffer) ) )
                OGRE_EXCEPT(
                    Exception::ERR_RENDERINGAPI_ERROR, 
                    "Error attaching depth buffer to render target",
                    "D3DDevice::createDepthBuffer" );

            // Log stencil buffer depth
            m_stencilBufferDepth = ddsd.ddpfPixelFormat.dwStencilBitDepth;

            StringUtil::StrStreamType str;
            str << "Depth-Buffer created (" << ddsd.ddpfPixelFormat.dwZBufferBitDepth
                << "-bit, " << m_stencilBufferDepth << "-bit stencil)";
            LogManager::getSingleton().logMessage( 
                LML_NORMAL, str.str());
            if (m_stencilBufferDepth == 0)
            {
                LogManager::getSingleton().logMessage("Warning: software stencilling " 
                    "in use, stencil operations will not be hardware accelerated.");
            }
        }
    }
	//////////////////////////////////////////////////////////////////////////
    bool D3DDevice::canMipmap(void) const
    {
        return (m_D3DDeviceDesc.dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_MIPNEAREST) > 0;
    }
	//////////////////////////////////////////////////////////////////////////
    bool D3DDevice::CanBilinearFilter(void) const
    {
        return (m_D3DDeviceDesc.dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_LINEAR) > 0;
    }
	//////////////////////////////////////////////////////////////////////////
    bool D3DDevice::CanTrilinearFilter(void) const
    {
        return (m_D3DDeviceDesc.dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_LINEARMIPLINEAR) > 0;
    }
	//////////////////////////////////////////////////////////////////////////
    unsigned int D3DDevice::RenderBitDepth(void) const
    {

        if (m_D3DDeviceDesc.dwDeviceRenderBitDepth & DDBD_32)
            return 32;
        else if (m_D3DDeviceDesc.dwDeviceRenderBitDepth & DDBD_24)
            return 24;
        else if (m_D3DDeviceDesc.dwDeviceRenderBitDepth & DDBD_16)
            return 16;
        else if (m_D3DDeviceDesc.dwDeviceRenderBitDepth & DDBD_8)
            return 8;
        else
            return 0;
    }
	//////////////////////////////////////////////////////////////////////////
    unsigned int D3DDevice::ZBufferBitDepth(void) const
    {
        switch(m_D3DDeviceDesc.dwDeviceZBufferBitDepth)
        {
        case DDBD_8:
            return 8;
        case DDBD_16:
            return 16;
        case DDBD_24:
            return 24;
        case DDBD_32:
            return 32;
        }

        return 0;

    }
	//////////////////////////////////////////////////////////////////////////
    bool D3DDevice::NeedsZBuffer(void) const
    {
        return m_needsZBuffer;
    }
	//////////////////////////////////////////////////////////////////////////
    bool D3DDevice::CanHWTransformAndLight(void) const
    {
        return (m_D3DDeviceDesc.dwDevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) > 0;
    }
	//////////////////////////////////////////////////////////////////////////
    unsigned int D3DDevice::MaxSinglePassTextureLayers(void) const
    {
        // The maximum number of texture layers the device can support in a singe pass

        return m_D3DDeviceDesc.wMaxSimultaneousTextures;
    }
	//////////////////////////////////////////////////////////////////////////
    ushort D3DDevice::StencilBufferBitDepth(void) const
    {
        return m_stencilBufferDepth;
    }
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
