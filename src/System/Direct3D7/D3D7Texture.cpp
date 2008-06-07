
#include "D3D7Texture.h"
#	include "D3D7HardwarePixelBuffer.h"
//#include "OgreException.h"
//#include "OgreImage.h"
//#include "OgreLogManager.h"
//#include "OgreHardwarePixelBuffer.h"
//#include "OgreRoot.h"

namespace Menge
{

    D3DX_SURFACEFORMAT D3DTexture::OgreFormat_to_D3DXFormat( PixelFormat format )
    {
		switch( format )
		{
		case PF_R8G8B8:
			return D3DX_SF_R8G8B8;
		case PF_A8R8G8B8:
			return D3DX_SF_A8R8G8B8;
		case PF_X8R8G8B8:
    		return D3DX_SF_X8R8G8B8;
		case PF_R5G6B5:
    		return D3DX_SF_R5G6B5;
		case PF_A4R4G4B4:
    		return D3DX_SF_A4R4G4B4;
		case PF_L8:
    		return D3DX_SF_L8;
		case PF_BYTE_LA: // Assume little endian
    		return D3DX_SF_A8L8;
		case PF_DXT1:
    		return D3DX_SF_DXT1;
		case PF_DXT3:
    		return D3DX_SF_DXT3;
		case PF_DXT5:
    		return D3DX_SF_DXT5;
		case PF_A8:
    		return D3DX_SF_A8;
		default:
			return D3DX_SF_UNKNOWN;
		}
    }

	PixelFormat D3DTexture::closestD3DXFormat( PixelFormat format )
	{
		if(OgreFormat_to_D3DXFormat(format) != D3DX_SF_UNKNOWN)
		{
			// Format is directly supported -- great!
			return format;
		}
		switch(format)
		{
		case PF_B5G6R5:
			return PF_R5G6B5;
		case PF_B8G8R8:
			return PF_R8G8B8;
		case PF_B8G8R8A8:
			return PF_A8R8G8B8;
		default:
			return PF_A8R8G8B8;
		}
	}

    bool D3DTexture::OgreFormat_to_DDPixelFormat( PixelFormat format, DDPIXELFORMAT & out )
    {
		D3DX_SURFACEFORMAT fmt = OgreFormat_to_D3DXFormat(format);
		if(fmt != D3DX_SF_UNKNOWN)
		{
			D3DXMakeDDPixelFormat(fmt, &out);
			return true;
		}
		return false;
#if 0
        memset( &out, 0, sizeof( DDPIXELFORMAT ) );
        out.dwSize = sizeof( DDPIXELFORMAT );
		if(PixelUtil::isNativeEndian(format))
		{
			int depths[4];
			uint32 masks[4];
			size_t elemBits = PixelUtil::getNumElemBits(format);
			PixelUtil::getBitDepths(format, depths);
			PixelUtil::getBitMasks(format, masks);

			if(PixelUtil::isLuminance(format))
			{
				out.dwFlags = DDPF_LUMINANCE;
				out.dwLuminanceBitCount = elemBits;
				out.dwLuminanceBitMask = masks[0];
				if(PixelUtil::hasAlpha(format))
				{
					out.dwFlags |= DDPF_ALPHAPIXELS;
					out.dwLuminanceAlphaBitMask = masks[3];
				}
			}
			else if(depths[0]==0 && depths[1]==0 && depths[2] == 0 && depths[3] != 0)
			{
				// Alpha only format
				out.dwFlags = DDPF_ALPHA;
				out.dwAlphaBitDepth = elemBits;
			}
			else
			{
				// RGB[A] format
				out.dwFlags = DDPF_RGB;
				out.dwRGBBitCount = elemBits;
				out.dwRBitMask = masks[0];
				out.dwGBitMask = masks[1];
				out.dwBBitMask = masks[2];
				
				if(PixelUtil::hasAlpha(format))
				{
					out.dwFlags |= DDPF_ALPHAPIXELS;
					out.dwRGBAlphaBitMask = masks[3];
				}
			}
		}
		else
		{
			switch( format )
			{
			case PF_BYTE_LA:
				// Assume little endian
				out.dwFlags = DDPF_LUMINANCE | DDPF_ALPHAPIXELS;
				out.dwLuminanceBitCount = 16;

				out.dwLuminanceAlphaBitMask = 0xff00;
				out.dwLuminanceBitMask = 0x00ff;
				break;
			default:
				// Error unknown format
				return false;
			}
		}
		return true;
#endif
	}

    //////////////////////////////////////////////////////////////////////////
    D3DTexture::D3DTexture( const String& _name, IDirect3DDevice7* lpDirect3dDevice )
        : m_D3DDevice( lpDirect3dDevice )
		//, m_restoring( false )
		, m_surface( NULL )
		, m_width( 0 )
		, m_height( 0 )
    {
        m_D3DDevice->AddRef();
    }
    //////////////////////////////////////////////////////////////////////////
    D3DTexture::~D3DTexture()
    {
        __safeRelease( &m_D3DDevice );
    }
    //////////////////////////////////////////////////////////////////////////
   /* void D3DTexture::loadImage3D( const Image imgs[] )
    {
        // Use OGRE its own codecs
		std::vector<const Image*> imagePtrs;
		imagePtrs.push_back(&imgs[0]);
		imagePtrs.push_back(&imgs[1]);
		imagePtrs.push_back(&imgs[2]);
		imagePtrs.push_back(&imgs[3]);
		imagePtrs.push_back(&imgs[4]);
		imagePtrs.push_back(&imgs[5]);
		_loadImages( imagePtrs );
	}*/
    //////////////////////////////////////////////////////////////////////////
    void D3DTexture::createInternalResourcesImpl( ETextureType _type, int _usage, bool _hasAlpha,
		std::size_t _srcWidth, std::size_t _srcHeight, std::size_t _mipmapsNum )
    {
		// Skip this if we're restoring device
		// We're using the generic load() but skipping init since DD can restore that
		//if ( !m_restoring )
		m_type = _type;
		m_hasAlpha = _hasAlpha;
		m_usage = _usage;
		{
			if ( m_type == TEX_TYPE_CUBE_MAP )
			{
				createSurface3D( _srcWidth, _srcHeight, _mipmapsNum );
			}
			else if( m_type == TEX_TYPE_2D )
			{
				createSurface2D( _srcWidth, _srcHeight, _mipmapsNum );
			}
			else
			{
				assert( 0 && "D3DTexture::createInternalResources -> Unknown texture type" );
			}
			createSurfaceList_( _mipmapsNum );
		}
    }
	//////////////////////////////////////////////////////////////////////////
	/*void D3DTexture::restoreFromLostDevice()
	{
		if (isLoaded() && !(mUsage & TU_RENDERTARGET))
		{
			m_restoring = true;
			// Mark as unloaded even though we're not so we can reload content
			//mIsLoaded = false;
			mLoadingState = LoadingState::LOADSTATE_UNLOADED;
			load();
			m_restoring = false;
		}
	}*/
    //////////////////////////////////////////////////////////////////////////
    /*void D3DTexture::loadImpl(void)
    {
		if( m_textureType == TEX_TYPE_CUBE_MAP )
		{
			constructCubeFaceNames_( m_name );
			Image imgs[6];
			for (int face = 0; face < 6; ++face)
			{
				imgs[face].load(m_cubeFaceNames[face], mGroup);
			}
			loadImage3D(imgs);

		}
		else if(mTextureType == TEX_TYPE_2D)
		{
			Image img;
			img.load(mName, mGroup);
			
			loadImage( img );
		}
		else
		{
			assert( 0 && "D3DTexture::loadImpl -> Unknown texture type" );
		}

    }*/
	//////////////////////////////////////////////////////////////////////////
	void D3DTexture::copyToTexture( TextureInterface* _target )
	{
		HRESULT hr;
		hr = static_cast<D3DTexture*>( _target )->getDDSurface()->Blt( NULL, m_surface, NULL, DDBLT_WAIT, NULL );
        if( FAILED( hr ) )
        {
			assert( ( hr == DDERR_SURFACELOST ) && "D3DTexture::copyToTexture -> Couldn't blit!" );
        }
	}
    //////////////////////////////////////////////////////////////////////////
    void D3DTexture::freeInternalResourcesImpl()
    {
        __safeRelease( &m_surface );
    }
	//////////////////////////////////////////////////////////////////////////
	void D3DTexture::chooseD3DFormat_( DDPIXELFORMAT &ddpf )
	{
		//mFormat = closestD3DXFormat(mFormat);
		//OgreFormat_to_DDPixelFormat(mFormat, ddpf);
		// The pixel format is always RGB
		//PixelUtil::getNumElemBits
		DWORD m_finalBpp = 32;

        ddpf.dwFlags = DDPF_RGB; 
        ddpf.dwRGBBitCount = m_finalBpp;
        
        if( m_hasAlpha )
        {
            ddpf.dwFlags |= DDPF_ALPHAPIXELS;
        }

        // Compute the bit masks
        if( m_finalBpp == 16 )
        {
            if( m_hasAlpha )
            {
                // 4-4-4-4 ARGB
                ddpf.dwRGBAlphaBitMask = 0xf000;
                ddpf.dwRBitMask        = 0x0f00;
                ddpf.dwGBitMask        = 0x00f0;
                ddpf.dwBBitMask        = 0x000f;
				m_format = PF_A4R4G4B4;
            }
            else
            {
                // 5-6-5 RGB
                ddpf.dwRBitMask = 0xf800;
                ddpf.dwGBitMask = 0x07e0;
                ddpf.dwBBitMask = 0x001f;
				m_format = PF_R5G6B5;
            }
        }
        else
        {
            if( m_hasAlpha )
            {
                // 8-8-8-8 ARGB
                ddpf.dwRGBAlphaBitMask = 0xff000000;
                ddpf.dwRBitMask        = 0x00ff0000;
                ddpf.dwGBitMask        = 0x0000ff00;
                ddpf.dwBBitMask        = 0x000000ff;
				m_format = PF_A8R8G8B8;
            }
            else
            {
                // 8-8-8 RGB
                ddpf.dwRBitMask        = 0xff0000;
                ddpf.dwGBitMask        = 0x00ff00;
                ddpf.dwBBitMask        = 0x0000ff;
				m_format = PF_X8R8G8B8;
            }
        }
	}
	//////////////////////////////////////////////////////////////////////////
	int Menge::D3DTexture::getPixelFormat()
	{
		return static_cast<int>( m_format );
	}
	//////////////////////////////////////////////////////////////////////////
	void D3DTexture::createSurface2D( std::size_t _srcWidth, std::size_t _srcHeight, std::size_t _mipmapsNum )
	{
        D3DDEVICEDESC7 ddDesc;
        ZeroMemory( &ddDesc, sizeof(D3DDEVICEDESC7) );

		HRESULT hr;
		hr = m_D3DDevice->GetCaps( &ddDesc );
        assert( SUCCEEDED( hr ) && "D3DTexture::createSurfaces -> Could not retrieve Direct3D Device caps." );

        // Create a surface descriptor.
        DDSURFACEDESC2 ddsd;
        D3DUtil_InitSurfaceDesc( ddsd );

        ddsd.dwSize          = sizeof( DDSURFACEDESC2 );
        ddsd.dwFlags         = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
        ddsd.ddsCaps.dwCaps  = DDSCAPS_TEXTURE;
        ddsd.ddsCaps.dwCaps2 = 0;
        ddsd.dwWidth         = _srcWidth;
        ddsd.dwHeight        = _srcHeight;

        // If the texture is a render target, set the corresponding flags
        if( m_usage & TU_RENDERTARGET )
        {
            ddsd.ddsCaps.dwCaps |= DDSCAPS_3DDEVICE | DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY;
            ddsd.ddsCaps.dwCaps2 = 0;
            _mipmapsNum = 0;
        }
        else
        {
            ddsd.ddsCaps.dwCaps2 = DDSCAPS2_D3DTEXTUREMANAGE;
        }
        
        // If we want to have mip-maps, set the flags. Note that if the
        // texture is the render target type mip-maps are automatically 
        // disabled.
        if( _mipmapsNum )
        {
            ddsd.dwFlags |= DDSD_MIPMAPCOUNT;
            ddsd.dwMipMapCount = _mipmapsNum;

            ddsd.ddsCaps.dwCaps |= DDSCAPS_MIPMAP | DDSCAPS_COMPLEX;            
        }

        chooseD3DFormat_( ddsd.ddpfPixelFormat );

        // Change texture size so that it is a power of 2, if needed.
        if( ddDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_POW2 )
        {
            for( ddsd.dwWidth = 1; _srcWidth > ddsd.dwWidth; ddsd.dwWidth <<= 1 );
            for( ddsd.dwHeight = 1; _srcHeight > ddsd.dwHeight; ddsd.dwHeight <<= 1 );
        }

        // Change texture size so that it is square, if needed. Note that we made it a
        // power of 2 in the above test, so here we just have to find the bigger dimension
        // and make it the side length.
        if( ddDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY )
        {
            if( ddsd.dwWidth > ddsd.dwHeight ) 
			{
                ddsd.dwHeight = ddsd.dwWidth;
			}
            else                               
			{
                ddsd.dwWidth  = ddsd.dwHeight;
			}
        }

        // Register the possibly modified dimensions.
        m_width = ddsd.dwWidth;
        m_height = ddsd.dwHeight;
        //mIsLoaded = true;
		//m_loadingState = LoadingState::LOADSTATE_LOADED;

        LPDIRECTDRAWSURFACE7 pddsRender;
        LPDIRECTDRAW7        pDD;

        /* Get a DirectDraw interface. */
        m_D3DDevice->GetRenderTarget( &pddsRender );
        pddsRender->GetDDInterface( (VOID**)&pDD );
        pddsRender->Release();

        if( m_usage & TU_RENDERTARGET )
        {
            // Get the pixel format of the primary surface - we need it because
            // render target surfaces need to have the same pixel format.
            IDirectDrawSurface7 * pddsPrimarySurface;
            pDD->GetGDISurface( &pddsPrimarySurface );
            pddsPrimarySurface->GetPixelFormat( &( ddsd.ddpfPixelFormat ) );
            pddsPrimarySurface->Release();
        }

		// Now create the surface.
		hr = pDD->CreateSurface( &ddsd, &m_surface, NULL );
        if( FAILED( hr ) )
        {
            pDD->Release();
            assert( 0 && "D3DTexture::createSurfaces -> Could not create DirectDraw surface." );
        }

        if( m_usage & TU_RENDERTARGET )
        {
            LPDIRECTDRAWSURFACE7 pddsZBuffer, pddsBackBuffer, pddsTexZBuffer;

            DDSCAPS2 ZBuffDDSCaps;
            DDSURFACEDESC2 ZBuffDDSD;

            ZBuffDDSCaps.dwCaps = DDSCAPS_ZBUFFER;
            ZBuffDDSCaps.dwCaps2 = ZBuffDDSCaps.dwCaps3 = ZBuffDDSCaps.dwCaps4 = 0;

            memset( &ZBuffDDSD, 0, sizeof( DDSURFACEDESC2 ) );
            ZBuffDDSD.dwSize = sizeof( DDSURFACEDESC2 );

            /* If the primary surface has an attached z-buffer, we need one for our texture
               too. Here, we get the primary surface's z-buffer format and we create a new
               Z-buffer that we attach to the our texture. */
            if( SUCCEEDED( m_D3DDevice->GetRenderTarget( &pddsBackBuffer ) ) )
            {
                if( SUCCEEDED( pddsBackBuffer->GetAttachedSurface( &ZBuffDDSCaps, &pddsZBuffer ) ) )
                {
                    pddsZBuffer->GetSurfaceDesc( &ZBuffDDSD );

                    /* Our new Z-buffer should have the size of the new render target. */
                    ZBuffDDSD.dwWidth = ddsd.dwWidth;
                    ZBuffDDSD.dwHeight = ddsd.dwHeight;

                    hr = pDD->CreateSurface( &ZBuffDDSD, &pddsTexZBuffer, NULL );
                    hr = m_surface->AddAttachedSurface( pddsTexZBuffer );

                    pddsBackBuffer->Release();
                    pddsZBuffer->Release();
                    pddsTexZBuffer->Release();
                }
            }
        }

        /* Release the DirectDraw interface used in the surface creation */
        pDD->Release();
    }
	//////////////////////////////////////////////////////////////////////////
	void D3DTexture::createSurface3D( std::size_t _srcWidth, std::size_t _srcHeight, std::size_t _mipmapsNum )
	{
        D3DDEVICEDESC7 ddDesc;
        ZeroMemory( &ddDesc, sizeof(D3DDEVICEDESC7) );

		HRESULT hr = m_D3DDevice->GetCaps( &ddDesc ); 
        assert( SUCCEEDED( hr ) && "D3DTexture::createSurfaces -> Could not retrieve Direct3D Device caps." );

        // Create a surface descriptor.
        DDSURFACEDESC2 ddsd;
        D3DUtil_InitSurfaceDesc( ddsd );

        ddsd.dwSize          = sizeof( DDSURFACEDESC2 );
        ddsd.dwFlags         = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
        ddsd.ddsCaps.dwCaps  = DDSCAPS_COMPLEX | DDSCAPS_TEXTURE;
        ddsd.ddsCaps.dwCaps2 = DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_ALLFACES;
        ddsd.dwWidth         = _srcWidth;
        ddsd.dwHeight        = _srcHeight;

        /* If the texture is a render target, set the corresponding flags */
        if( m_usage & TU_RENDERTARGET )
        {
            ddsd.ddsCaps.dwCaps |= DDSCAPS_3DDEVICE | DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY;
            _mipmapsNum = 0;
        }
        else
        {
            ddsd.ddsCaps.dwCaps2 |= DDSCAPS2_D3DTEXTUREMANAGE;
        }
        
        // If we want to have mip-maps, set the flags. Note that if the
        // texture is the render target type mip-maps are automatically 
        // disabled.
        if( _mipmapsNum )
        {
            ddsd.dwFlags |= DDSD_MIPMAPCOUNT;
            ddsd.dwMipMapCount = _mipmapsNum;

            ddsd.ddsCaps.dwCaps |= DDSCAPS_MIPMAP;            
        }

        chooseD3DFormat_( ddsd.ddpfPixelFormat );

        // Change texture size so that it is a power of 2, if needed.
        if( ddDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_POW2 )
        {
            for( ddsd.dwWidth = 1; _srcWidth > ddsd.dwWidth; ddsd.dwWidth <<= 1 );
            for( ddsd.dwHeight = 1; _srcHeight > ddsd.dwHeight; ddsd.dwHeight <<= 1 );
        }

        // Change texture size so that it is square, if needed. Note that we made it a
        // power of 2 in the above test, so here we just have to find the bigger dimension
        // and make it the side length.
        if( ddDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY )
        {
            if( ddsd.dwWidth > ddsd.dwHeight ) 
			{
                ddsd.dwHeight = ddsd.dwWidth;
			}
            else                               
			{
                ddsd.dwWidth  = ddsd.dwHeight;
			}
        }

        // Register the possibly modified dimensions. 
        m_width = ddsd.dwWidth;
        m_height = ddsd.dwHeight;
        //mIsLoaded = true;
		//m_loadingState = LoadingState::LOADSTATE_LOADED;


        LPDIRECTDRAWSURFACE7 pddsRender;
        LPDIRECTDRAW7        pDD;

        /* Get a DirectDraw interface. */
        m_D3DDevice->GetRenderTarget( &pddsRender );
        pddsRender->GetDDInterface( (VOID**)&pDD );
        pddsRender->Release();

        if( m_usage & TU_RENDERTARGET )
        {
            // Get the pixel format of the primary surface - we need it because
            // render target surfaces need to have the same pixel format.
            IDirectDrawSurface7 * pddsPrimarySurface;
            pDD->GetGDISurface( &pddsPrimarySurface );
            pddsPrimarySurface->GetPixelFormat( &( ddsd.ddpfPixelFormat ) );
            pddsPrimarySurface->Release();
        }

        // Now create the surface.
		hr = pDD->CreateSurface( &ddsd, &m_surface, NULL ) ;
        if( FAILED( hr ) )
        {
            pDD->Release();
            assert( 0 && "D3DTexture::createSurfaces -> Could not create DirectDraw surface." );
        }

        if( m_usage & TU_RENDERTARGET )
        {
            LPDIRECTDRAWSURFACE7 pddsZBuffer, pddsBackBuffer, pddsTexZBuffer;

            DDSCAPS2 ZBuffDDSCaps;
            DDSURFACEDESC2 ZBuffDDSD;

            ZBuffDDSCaps.dwCaps = DDSCAPS_ZBUFFER;
            ZBuffDDSCaps.dwCaps2 = ZBuffDDSCaps.dwCaps3 = ZBuffDDSCaps.dwCaps4 = 0;

            memset( &ZBuffDDSD, 0, sizeof( DDSURFACEDESC2 ) );
            ZBuffDDSD.dwSize = sizeof( DDSURFACEDESC2 );

            /* If the primary surface has an attached z-buffer, we need one for our texture
               too. Here, we get the primary surface's z-buffer format and we create a new
               Z-buffer that we attach to the our texture. */
            if( SUCCEEDED( m_D3DDevice->GetRenderTarget( &pddsBackBuffer ) ) )
            {
                if( SUCCEEDED( pddsBackBuffer->GetAttachedSurface( &ZBuffDDSCaps, &pddsZBuffer ) ) )
                {
                    pddsZBuffer->GetSurfaceDesc( &ZBuffDDSD );

                    /* Our new Z-buffer should have the size of the new render target. */
                    ZBuffDDSD.dwWidth = ddsd.dwWidth;
                    ZBuffDDSD.dwHeight = ddsd.dwHeight;

                    hr = pDD->CreateSurface( &ZBuffDDSD, &pddsTexZBuffer, NULL );
                    hr = m_surface->AddAttachedSurface( pddsTexZBuffer );

                    pddsBackBuffer->Release();
                    pddsZBuffer->Release();
                    pddsTexZBuffer->Release();
                }
            }
        }

        // Release the DirectDraw interface used in the surface creation
        pDD->Release();
	}
    //////////////////////////////////////////////////////////////////////////
    LPDIRECTDRAWSURFACE7 D3DTexture::getDDSurface()
    {
        return m_surface;
    }
	//////////////////////////////////////////////////////////////////////////
	/*void D3DTexture::constructCubeFaceNames_( const String& _name )
	{
		// the suffixes
		String suffixes[6] = {"_rt", "_lf", "_up", "_dn", "_fr", "_bk"};
		std::size_t pos = -1;

		String ext; // the extension
		String baseName; // the base name
		String fakeName = _name; // the 'fake' name, temp. holder

		// first find the base name
		pos = fakeName.find_last_of(".");
		assert( ( pos != -1 ) && "D3D9Texture::_constructCubeFaceNames -> Invalid cube texture base name" );

		baseName = fakeName.substr(0, pos);
		ext = fakeName.substr(pos);

		// construct the full 6 faces file names from the baseName, suffixes and extension
		for (size_t i = 0; i < 6; ++i)
		{
			m_cubeFaceNames[i] = baseName + suffixes[i] + ext;
		}
	}*/
	//////////////////////////////////////////////////////////////////////////
	HardwarePixelBufferInterface* D3DTexture::getBuffer( std::size_t _idx )
	{
		//assert( _idx < m_surfaceList.size() );
		return NULL;
		//return m_surfaceList[_idx];
	}
    //////////////////////////////////////////////////////////////////////////
	// Macro to hide ugly cast
	//#define GETLEVEL( _face, _mip ) \
	// 	static_cast<D3D7HardwarePixelBuffer*>( m_surfaceList[face * ( _mipmapsNum + 1) + _mip])
	//void D3DTexture::createSurfaceList_( std::size_t _mipmapsNum )
	//{
	//	HRESULT hr;
	//	std::size_t face;
	//	m_surfaceList.clear();
	//	D3D7HardwarePixelBuffer* buffer;

	//	// Get real number of mipmaps
	//	DDSURFACEDESC2 desc;
	//	desc.dwSize = sizeof(DDSURFACEDESC2);
	//	hr = m_surface->GetSurfaceDesc(&desc);
	//	assert( ( hr == D3D_OK ) && "D3DTexture::_createSurfaceList() -> Could not get surface information" );

	//	/*if( desc.dwFlags & DDSD_MIPMAPCOUNT )
	//	{
	//		m_numMipmaps = desc.dwMipMapCount;
	//	}
	//	else
	//	{
	//		m_numMipmaps = 0;
	//	}*/

	//	// Create the list
	//	unsigned int bufusage;
	//	if( ( m_usage & TU_DYNAMIC ) )
	//	{
	//		bufusage = HBU_DYNAMIC;
	//	}
	//	else
	//	{
	//		bufusage = HBU_STATIC;
	//	}
	//	if( m_usage & TU_RENDERTARGET )
	//	{
	//		bufusage |= TU_RENDERTARGET;
	//	}

	//	bool updateOldList = m_surfaceList.size() == ( getNumFaces() * ( _mipmapsNum + 1 ) );
	//	if( !updateOldList )
	//	{
	//		// Create new list of surfaces
	//		m_surfaceList.clear();
	//		for( std::size_t face = 0; face < getNumFaces(); ++face )
	//		{
	//			for( std::size_t mip = 0; mip <= _mipmapsNum; ++mip )
	//			{
	//				buffer = new D3D7HardwarePixelBuffer( (EHardwareBufferUsage)bufusage, m_D3DDevice );
	//				m_surfaceList.push_back( static_cast<HardwarePixelBufferInterface*>( buffer ) );
	//			}
	//		}
	//	}

	//	/*for(face=0; face < getNumFaces(); ++face)
	//	{
	//		for(size_t mip=0; mip<=mNumMipmaps; ++mip)
	//		{
	//			mSurfaceList.push_back(
	//				HardwarePixelBufferSharedPtr(
	//					new D3D7HardwarePixelBuffer((HardwareBuffer::Usage)bufusage, mD3DDevice)
	//				)
	//			);
	//		}
	//	}*/

	//	// Enumerate all surfaces
 //       for (face = 0; face < getNumFaces(); ++face)
	//	{
	//		LPDIRECTDRAWSURFACE7 pCubeFace;
	//		if( m_type == TEX_TYPE_CUBE_MAP)
	//		{
	//			// Get cube map face to blit to
	//			DDSCAPS2 cubeCaps;
	//			ZeroMemory(&cubeCaps, sizeof(DDSCAPS2));
	//			switch (face)
	//			{
	//			case 0:
	//				// left
	//				cubeCaps.dwCaps2 = DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEX;
	//				break;
	//			case 1:
	//				// right
	//				cubeCaps.dwCaps2 = DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEX;
	//				break;
	//			case 2:
	//				// up
	//				cubeCaps.dwCaps2 = DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEY;
	//				break;
	//			case 3:
	//				// down
	//				cubeCaps.dwCaps2 = DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEY;
	//				break;
	//			case 4:
	//				// front - NB DirectX is backwards
	//				cubeCaps.dwCaps2 = DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEZ;
	//				break;
	//			case 5:
	//				// back - NB DirectX is backwards
	//				cubeCaps.dwCaps2 = DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEZ;
	//				break;
	//			}
	//			
	//			hr = m_surface->GetAttachedSurface( &cubeCaps, &pCubeFace );
	//			assert( SUCCEEDED( hr ) &&  "D3DTexture::_createSurfaceList -> Error getting cube face surface." );

	//		}
	//		else
	//		{
	//			pCubeFace = m_surface;
	//			pCubeFace->AddRef();
	//		}

	//		GETLEVEL(face, 0)->bind( pCubeFace, m_format );

	//		LPDIRECTDRAWSURFACE7 ddsMipLevel, ddsNextLevel;
	//		DDSCAPS2 ddsCaps;
	//		HRESULT mipRes = DD_OK;
	//		size_t mipLevel = 1;

	//		ZeroMemory(&ddsCaps, sizeof(DDSCAPS2));
	//		ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;

	//		// Get the base level and increae the reference count. 
	//		ddsMipLevel = pCubeFace;
	//		ddsMipLevel->AddRef();

	//		/// While we can get a next level in the mip-map chain. 
	//		while( ddsMipLevel->GetAttachedSurface( &ddsCaps, &ddsNextLevel ) == DD_OK )
	//		{
	//			GETLEVEL(face, mipLevel)->bind( ddsNextLevel, m_format );
	//			// Release the current level and get the next one, incrementing the mip depth. 
	//			ddsMipLevel->Release();
	//			ddsMipLevel = ddsNextLevel;
	//			mipLevel++;
	//		}

	//		// Release the last mip-map level surface. 
	//		ddsMipLevel->Release();
	//		// Release face reference
	//		pCubeFace->Release();
	//	}

	//	// Set autogeneration of mipmaps for each face of the texture, if it is enabled
	//	if( _mipmapsNum > 0 && ( m_usage & TU_AUTOMIPMAP ) ) 
	//	{
	//		for( std::size_t face = 0; face < getNumFaces(); ++face )
	//		{
	//			GETLEVEL(face, 0)->_setMipmapping( true );
	//		}
	//	}
	//}
	//#undef GETLEVEL
	//////////////////////////////////////////////////////////////////////////
	std::size_t D3DTexture::getWidth()
	{
		return m_width;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t D3DTexture::getHeight()
	{
		return m_height;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t D3DTexture::getNumFaces() const
	{
		return m_type == TEX_TYPE_CUBE_MAP ? 6 : 1;
	}
	//////////////////////////////////////////////////////////////////////////
	HardwarePixelBufferInterface* D3DTexture::createHardwarePixelBuffer( EHardwareBufferUsage _usage )
	{
		//D3D7HardwarePixelBuffer* buffer = NULL;
		//buffer = new D3D7HardwarePixelBuffer( _usage, m_D3DDevice );
		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
