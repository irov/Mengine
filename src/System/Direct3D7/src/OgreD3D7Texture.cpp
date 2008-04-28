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
#include "OgreD3D7Texture.h"
#include "OgreException.h"
#include "OgreImage.h"
#include "OgreLogManager.h"
#include "OgreHardwarePixelBuffer.h"
#include "OgreRoot.h"

namespace Ogre {

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

    //---------------------------------------------------------------------------------------------
    D3DTexture::D3DTexture(ResourceManager* creator, const String& name, 
        ResourceHandle handle, const String& group, bool isManual, 
        ManualResourceLoader* loader, IDirect3DDevice7 * lpDirect3dDevice)
        :Texture(creator, name, handle, group, isManual, loader),
        mD3DDevice(lpDirect3dDevice), mRestoring(false)
		, mSurface( NULL )
    {
        mD3DDevice->AddRef();
    }
    //---------------------------------------------------------------------------------------------
    D3DTexture::~D3DTexture()
    {
        __safeRelease( &mD3DDevice );
        // have to call this here reather than in Resource destructor
        // since calling virtual methods in base destructors causes crash
		if (isLoaded())
		{
            unload(); 
		}
		else
		{
			freeInternalResources();
		}
    }
	
    //---------------------------------------------------------------------------------------------
    /*void D3DTexture::loadImage( const Image & img )
    {
        // Use OGRE its own codecs
		std::vector<const Image*> imagePtrs;
		imagePtrs.push_back(&img);
		_loadImages( imagePtrs );
	}*/
    //---------------------------------------------------------------------------------------------
    void D3DTexture::loadImage3D( const Image imgs[] )
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
	}
    //---------------------------------------------------------------------------------------------
    void D3DTexture::createInternalResourcesImpl(void)
    {
		// Skip this if we're restoring device
		// We're using the generic load() but skipping init since DD can restore that
		if (!mRestoring)
		{
			if (mTextureType == TEX_TYPE_CUBE_MAP)
			{
				createSurface3D();
			}
			else if(mTextureType == TEX_TYPE_2D)
			{
				createSurface2D();
			}
			else
			{
				OGRE_EXCEPT( Exception::ERR_INTERNAL_ERROR, "Unknown texture type", "D3DTexture::createInternalResources" );
			}
			_createSurfaceList();
		}
    }
	//---------------------------------------------------------------------------------------------
	void D3DTexture::restoreFromLostDevice(void)
	{
		if (isLoaded() && !(mUsage & TU_RENDERTARGET))
		{
			mRestoring = true;
			// Mark as unloaded even though we're not so we can reload content
			//mIsLoaded = false;
			mLoadingState = LoadingState::LOADSTATE_UNLOADED;
			load();
			mRestoring = false;
		}
	}
    //---------------------------------------------------------------------------------------------
    void D3DTexture::loadImpl(void)
    {
		if (mTextureType == TEX_TYPE_CUBE_MAP)
		{
			_constructCubeFaceNames(mName);
			Image imgs[6];
			for (int face = 0; face < 6; ++face)
			{
				imgs[face].load(mCubeFaceNames[face], mGroup);
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
			OGRE_EXCEPT( Exception::ERR_INTERNAL_ERROR, "Unknown texture type", "D3DTexture::loadImpl" );
		}

    }
	//---------------------------------------------------------------------------------------------
	void D3DTexture::copyToTexture(TexturePtr& target )
	{
		HRESULT hr;
        if( FAILED( hr = ((D3DTexture*)(target.get()))->getDDSurface()->Blt( NULL, mSurface, NULL, DDBLT_WAIT, NULL ) ) )
        {
			if(hr != DDERR_SURFACELOST)
				OGRE_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR, "Couldn't blit!", "D3DTexture::copyToTexture" );
        }
	}

    //---------------------------------------------------------------------------------------------
    void D3DTexture::freeInternalResourcesImpl()
    {
        __safeRelease( &mSurface );
    }
	//---------------------------------------------------------------------------------------------
	void D3DTexture::_chooseD3DFormat(DDPIXELFORMAT &ddpf)
	{
		//mFormat = closestD3DXFormat(mFormat);
		//OgreFormat_to_DDPixelFormat(mFormat, ddpf);
		/* The pixel format is always RGB */

		DWORD mFinalBpp = 32;

        ddpf.dwFlags = DDPF_RGB; 
        ddpf.dwRGBBitCount = mFinalBpp;
        
        if( hasAlpha() )
        {
            ddpf.dwFlags |= DDPF_ALPHAPIXELS;
        }

        /* Compute the bit masks */
        if( mFinalBpp == 16 )
        {
            if( hasAlpha() )
            {
                /* 4-4-4-4 ARGB */
                ddpf.dwRGBAlphaBitMask = 0xf000;
                ddpf.dwRBitMask        = 0x0f00;
                ddpf.dwGBitMask        = 0x00f0;
                ddpf.dwBBitMask        = 0x000f;
				mFormat = PF_A4R4G4B4;
            }
            else
            {
                /* 5-6-5 RGB */
                ddpf.dwRBitMask = 0xf800;
                ddpf.dwGBitMask = 0x07e0;
                ddpf.dwBBitMask = 0x001f;
				mFormat = PF_R5G6B5;
            }
        }
        else
        {
            if( hasAlpha() )
            {
                /* 8-8-8-8 ARGB */
                ddpf.dwRGBAlphaBitMask = 0xff000000;
                ddpf.dwRBitMask        = 0x00ff0000;
                ddpf.dwGBitMask        = 0x0000ff00;
                ddpf.dwBBitMask        = 0x000000ff;
				mFormat = PF_A8R8G8B8;
            }
            else
            {
                /* 8-8-8 RGB */
                ddpf.dwRBitMask        = 0xff0000;
                ddpf.dwGBitMask        = 0x00ff00;
                ddpf.dwBBitMask        = 0x0000ff;
				mFormat = PF_X8R8G8B8;
            }
        }
	}
	//---------------------------------------------------------------------------------------------
	void D3DTexture::createSurface2D(void)
	{
        D3DDEVICEDESC7 ddDesc;
        ZeroMemory( &ddDesc, sizeof(D3DDEVICEDESC7) );

        if( FAILED( mD3DDevice->GetCaps( &ddDesc ) ) )
            OGRE_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR, 
                    "Could not retrieve Direct3D Device caps.",
                    "D3DTexture::createSurfaces" );

        // Create a surface descriptor.
        DDSURFACEDESC2 ddsd;
        D3DUtil_InitSurfaceDesc( ddsd );

        ddsd.dwSize          = sizeof(DDSURFACEDESC2);
        ddsd.dwFlags         = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
        ddsd.ddsCaps.dwCaps  = DDSCAPS_TEXTURE;
        ddsd.ddsCaps.dwCaps2 = 0;
        ddsd.dwWidth         = mSrcWidth;
        ddsd.dwHeight        = mSrcHeight;

        /* If the texture is a render target, set the corresponding flags */
        if( mUsage & TU_RENDERTARGET )
        {
            ddsd.ddsCaps.dwCaps |= DDSCAPS_3DDEVICE | DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY;
            ddsd.ddsCaps.dwCaps2 = 0;
            mNumRequestedMipmaps = 0;
        }
        else
        {
            ddsd.ddsCaps.dwCaps2 = DDSCAPS2_D3DTEXTUREMANAGE;
        }
        
        /* If we want to have mip-maps, set the flags. Note that if the
           texture is the render target type mip-maps are automatically 
           disabled. */
        if( mNumRequestedMipmaps )
        {
            ddsd.dwFlags |= DDSD_MIPMAPCOUNT;
            ddsd.dwMipMapCount = mNumRequestedMipmaps;

            ddsd.ddsCaps.dwCaps |= DDSCAPS_MIPMAP | DDSCAPS_COMPLEX;            
        }

        _chooseD3DFormat(ddsd.ddpfPixelFormat);

        /* Change texture size so that it is a power of 2, if needed. */
        if( ddDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_POW2 )
        {
            for( ddsd.dwWidth = 1; mSrcWidth > ddsd.dwWidth; ddsd.dwWidth <<= 1 );
            for( ddsd.dwHeight = 1; mSrcHeight > ddsd.dwHeight; ddsd.dwHeight <<= 1 );
        }

        /* Change texture size so that it is square, if needed. Note that we made it a
           power of 2 in the above test, so here we just have to find the bigger dimension
           and make it the side length. */
        if( ddDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY )
        {
            if( ddsd.dwWidth > ddsd.dwHeight ) 
                ddsd.dwHeight = ddsd.dwWidth;
            else                               
                ddsd.dwWidth  = ddsd.dwHeight;
        }

        /* Register the possibly modified dimensions. */
        mWidth = ddsd.dwWidth;
        mHeight = ddsd.dwHeight;
        //mIsLoaded = true;
		mLoadingState = LoadingState::LOADSTATE_LOADED;

        LPDIRECTDRAWSURFACE7 pddsRender;
        LPDIRECTDRAW7        pDD;

        /* Get a DirectDraw interface. */
        mD3DDevice->GetRenderTarget( &pddsRender );
        pddsRender->GetDDInterface( (VOID**)&pDD );
        pddsRender->Release();

        if( mUsage & TU_RENDERTARGET )
        {
            /* Get the pixel format of the primary surface - we need it because
               render target surfaces need to have the same pixel format. */
            IDirectDrawSurface7 * pddsPrimarySurface;
            pDD->GetGDISurface( &pddsPrimarySurface );
            pddsPrimarySurface->GetPixelFormat( &( ddsd.ddpfPixelFormat ) );
            pddsPrimarySurface->Release();
        }

        HRESULT hr;

		/* Now create the surface. */
        if( FAILED( hr = pDD->CreateSurface( &ddsd, &mSurface, NULL ) ) )
        {
            pDD->Release();
            OGRE_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR,
                "Could not create DirectDraw surface.",
                "D3DTexture::createSurfaces" );
        }

        if( mUsage & TU_RENDERTARGET )
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
            if( SUCCEEDED( mD3DDevice->GetRenderTarget( &pddsBackBuffer ) ) )
            {
                if( SUCCEEDED( pddsBackBuffer->GetAttachedSurface( &ZBuffDDSCaps, &pddsZBuffer ) ) )
                {
                    pddsZBuffer->GetSurfaceDesc( &ZBuffDDSD );

                    /* Our new Z-buffer should have the size of the new render target. */
                    ZBuffDDSD.dwWidth = ddsd.dwWidth;
                    ZBuffDDSD.dwHeight = ddsd.dwHeight;

                    hr = pDD->CreateSurface( &ZBuffDDSD, &pddsTexZBuffer, NULL );
                    hr = mSurface->AddAttachedSurface( pddsTexZBuffer );

                    pddsBackBuffer->Release();
                    pddsZBuffer->Release();
                    pddsTexZBuffer->Release();
                }
            }
        }

        /* Release the DirectDraw interface used in the surface creation */
        pDD->Release();
    }
	//---------------------------------------------------------------------------------------------
	void D3DTexture::createSurface3D(void)
	{
        D3DDEVICEDESC7 ddDesc;
        ZeroMemory( &ddDesc, sizeof(D3DDEVICEDESC7) );

        if( FAILED( mD3DDevice->GetCaps( &ddDesc ) ) )
            OGRE_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR, 
                    "Could not retrieve Direct3D Device caps.",
                    "D3DTexture::createSurfaces" );

        // Create a surface descriptor.
        DDSURFACEDESC2 ddsd;
        D3DUtil_InitSurfaceDesc( ddsd );

        ddsd.dwSize          = sizeof(DDSURFACEDESC2);
        ddsd.dwFlags         = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
        ddsd.ddsCaps.dwCaps  = DDSCAPS_COMPLEX | DDSCAPS_TEXTURE;
        ddsd.ddsCaps.dwCaps2 = DDSCAPS2_CUBEMAP|DDSCAPS2_CUBEMAP_ALLFACES;
        ddsd.dwWidth         = mSrcWidth;
        ddsd.dwHeight        = mSrcHeight;

        /* If the texture is a render target, set the corresponding flags */
        if( mUsage & TU_RENDERTARGET )
        {
            ddsd.ddsCaps.dwCaps |= DDSCAPS_3DDEVICE | DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY;
            mNumRequestedMipmaps = 0;
        }
        else
        {
            ddsd.ddsCaps.dwCaps2 |= DDSCAPS2_D3DTEXTUREMANAGE;
        }
        
        /* If we want to have mip-maps, set the flags. Note that if the
           texture is the render target type mip-maps are automatically 
           disabled. */
        if( mNumRequestedMipmaps )
        {
            ddsd.dwFlags |= DDSD_MIPMAPCOUNT;
            ddsd.dwMipMapCount = mNumRequestedMipmaps;

            ddsd.ddsCaps.dwCaps |= DDSCAPS_MIPMAP;            
        }

        _chooseD3DFormat(ddsd.ddpfPixelFormat);

        /* Change texture size so that it is a power of 2, if needed. */
        if( ddDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_POW2 )
        {
            for( ddsd.dwWidth = 1; mSrcWidth > ddsd.dwWidth; ddsd.dwWidth <<= 1 );
            for( ddsd.dwHeight = 1; mSrcHeight > ddsd.dwHeight; ddsd.dwHeight <<= 1 );
        }

        /* Change texture size so that it is square, if needed. Note that we made it a
           power of 2 in the above test, so here we just have to find the bigger dimension
           and make it the side length. */
        if( ddDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY )
        {
            if( ddsd.dwWidth > ddsd.dwHeight ) 
                ddsd.dwHeight = ddsd.dwWidth;
            else                               
                ddsd.dwWidth  = ddsd.dwHeight;
        }

        /* Register the possibly modified dimensions. */
        mWidth = ddsd.dwWidth;
        mHeight = ddsd.dwHeight;
        //mIsLoaded = true;
		mLoadingState = LoadingState::LOADSTATE_LOADED;


        LPDIRECTDRAWSURFACE7 pddsRender;
        LPDIRECTDRAW7        pDD;

        /* Get a DirectDraw interface. */
        mD3DDevice->GetRenderTarget( &pddsRender );
        pddsRender->GetDDInterface( (VOID**)&pDD );
        pddsRender->Release();

        if( mUsage & TU_RENDERTARGET )
        {
            /* Get the pixel format of the primary surface - we need it because
               render target surfaces need to have the same pixel format. */
            IDirectDrawSurface7 * pddsPrimarySurface;
            pDD->GetGDISurface( &pddsPrimarySurface );
            pddsPrimarySurface->GetPixelFormat( &( ddsd.ddpfPixelFormat ) );
            pddsPrimarySurface->Release();
        }

        HRESULT hr;

        /* Now create the surface. */
        if( FAILED( hr = pDD->CreateSurface( &ddsd, &mSurface, NULL ) ) )
        {
            pDD->Release();
            OGRE_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR,
                "Could not create DirectDraw surface.",
                "D3DTexture::createSurfaces" );
        }


        if( mUsage & TU_RENDERTARGET )
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
            if( SUCCEEDED( mD3DDevice->GetRenderTarget( &pddsBackBuffer ) ) )
            {
                if( SUCCEEDED( pddsBackBuffer->GetAttachedSurface( &ZBuffDDSCaps, &pddsZBuffer ) ) )
                {
                    pddsZBuffer->GetSurfaceDesc( &ZBuffDDSD );

                    /* Our new Z-buffer should have the size of the new render target. */
                    ZBuffDDSD.dwWidth = ddsd.dwWidth;
                    ZBuffDDSD.dwHeight = ddsd.dwHeight;

                    hr = pDD->CreateSurface( &ZBuffDDSD, &pddsTexZBuffer, NULL );
                    hr = mSurface->AddAttachedSurface( pddsTexZBuffer );

                    pddsBackBuffer->Release();
                    pddsZBuffer->Release();
                    pddsTexZBuffer->Release();
                }
            }
        }

        /* Release the DirectDraw interface used in the surface creation */
        pDD->Release();
	}
    //---------------------------------------------------------------------------------------------
    LPDIRECTDRAWSURFACE7 D3DTexture::getDDSurface(void)
    {
        return mSurface;
    }
	void D3DTexture::_constructCubeFaceNames(const String name)
	{
		// the suffixes
		String suffixes[6] = {"_rt", "_lf", "_up", "_dn", "_fr", "_bk"};
		size_t pos = -1;

		String ext; // the extension
		String baseName; // the base name
		String fakeName = name; // the 'fake' name, temp. holder

		// first find the base name
		pos = fakeName.find_last_of(".");
		if (pos == -1)
		{
			OGRE_EXCEPT( Exception::ERR_INTERNAL_ERROR, "Invalid cube texture base name", "D3D9Texture::_constructCubeFaceNames" );
		}

		baseName = fakeName.substr(0, pos);
		ext = fakeName.substr(pos);

		// construct the full 6 faces file names from the baseName, suffixes and extension
		for (size_t i = 0; i < 6; ++i)
			mCubeFaceNames[i] = baseName + suffixes[i] + ext;
	}
	//---------------------------------------------------------------------------------------------
	HardwarePixelBufferSharedPtr D3DTexture::getBuffer(size_t face, size_t mipmap)
	{
		if(face >= getNumFaces())
			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "A three dimensional cube has six faces",
					"D3D9Texture::getBuffer");
		if(mipmap > getNumMipmaps())
			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Mipmap index out of range",
					"D3D9Texture::getBuffer");
		int idx = face*(mNumMipmaps+1) + mipmap;
		assert(idx < mSurfaceList.size());
		return mSurfaceList[idx];
	}

    //---------------------------------------------------------------------------------------------
	// Macro to hide ugly cast
	#define GETLEVEL(face,mip) \
	 	static_cast<D3D7HardwarePixelBuffer*>(mSurfaceList[face*(mNumMipmaps+1)+mip].get())
	void D3DTexture::_createSurfaceList()
	{
		HRESULT hr;
		size_t face;
		mSurfaceList.clear();
		D3D7HardwarePixelBuffer *buffer;

		// Get real number of mipmaps
		DDSURFACEDESC2 desc;
		desc.dwSize = sizeof(DDSURFACEDESC2);
		if(mSurface->GetSurfaceDesc(&desc) != D3D_OK)
			OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Could not get surface information",
			"D3DTexture::_createSurfaceList()");
		if(desc.dwFlags & DDSD_MIPMAPCOUNT)
			mNumMipmaps = desc.dwMipMapCount;
		else
			mNumMipmaps = 0;

		// Create the list
		unsigned int bufusage;
		if ((mUsage & TU_DYNAMIC))
		{
			bufusage = HardwareBuffer::HBU_DYNAMIC;
		}
		else
		{
			bufusage = HardwareBuffer::HBU_STATIC;
		}
		if (mUsage & TU_RENDERTARGET)
		{
			bufusage |= TU_RENDERTARGET;
		}

		bool updateOldList = mSurfaceList.size() == (getNumFaces() * (mNumMipmaps + 1));
		if(!updateOldList)
		{
			// Create new list of surfaces
			mSurfaceList.clear();
			for(size_t face=0; face<getNumFaces(); ++face)
			{
				for(size_t mip=0; mip<=mNumMipmaps; ++mip)
				{
					buffer = new D3D7HardwarePixelBuffer((HardwareBuffer::Usage)bufusage, mD3DDevice);
					mSurfaceList.push_back(
						HardwarePixelBufferSharedPtr(buffer)
						);
				}
			}
		}

		/*for(face=0; face < getNumFaces(); ++face)
		{
			for(size_t mip=0; mip<=mNumMipmaps; ++mip)
			{
				mSurfaceList.push_back(
					HardwarePixelBufferSharedPtr(
						new D3D7HardwarePixelBuffer((HardwareBuffer::Usage)bufusage, mD3DDevice)
					)
				);
			}
		}*/

		// Enumerate all surfaces
        for (face = 0; face < getNumFaces(); ++face)
		{
			LPDIRECTDRAWSURFACE7 pCubeFace;
			if(mTextureType == TEX_TYPE_CUBE_MAP)
			{
				// Get cube map face to blit to
				DDSCAPS2 cubeCaps;
				ZeroMemory(&cubeCaps, sizeof(DDSCAPS2));
				switch (face)
				{
				case 0:
					// left
					cubeCaps.dwCaps2 = DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEX;
					break;
				case 1:
					// right
					cubeCaps.dwCaps2 = DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEX;
					break;
				case 2:
					// up
					cubeCaps.dwCaps2 = DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEY;
					break;
				case 3:
					// down
					cubeCaps.dwCaps2 = DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEY;
					break;
				case 4:
					// front - NB DirectX is backwards
					cubeCaps.dwCaps2 = DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEZ;
					break;
				case 5:
					// back - NB DirectX is backwards
					cubeCaps.dwCaps2 = DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEZ;
					break;
				}

				if (FAILED(hr = mSurface->GetAttachedSurface( &cubeCaps, &pCubeFace)))
				{
					OGRE_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR, "Error getting cube face surface.", "D3DTexture::_createSurfaceList" );
				}	
			}
			else
			{
				pCubeFace = mSurface;
				pCubeFace->AddRef();
			}

			GETLEVEL(face, 0)->bind(pCubeFace, mFormat);

			LPDIRECTDRAWSURFACE7 ddsMipLevel, ddsNextLevel;
			DDSCAPS2 ddsCaps;
			HRESULT mipRes = DD_OK;
			size_t mipLevel = 1;

			ZeroMemory(&ddsCaps, sizeof(DDSCAPS2));
			ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;

			// Get the base level and increae the reference count. 
			ddsMipLevel = pCubeFace;
			ddsMipLevel->AddRef();

			/// While we can get a next level in the mip-map chain. 
			while( ddsMipLevel->GetAttachedSurface( &ddsCaps, &ddsNextLevel ) == DD_OK )
			{
				GETLEVEL(face, mipLevel)->bind(ddsNextLevel, mFormat);
				// Release the current level and get the next one, incrementing the mip depth. 
				ddsMipLevel->Release();
				ddsMipLevel = ddsNextLevel;
				mipLevel++;
			}

			// Release the last mip-map level surface. 
			ddsMipLevel->Release();
			// Release face reference
			pCubeFace->Release();
		}

		// Set autogeneration of mipmaps for each face of the texture, if it is enabled
		if(mNumMipmaps>0 && (mUsage & TU_AUTOMIPMAP)) 
		{
			for(size_t face=0; face<getNumFaces(); ++face)
			{
				GETLEVEL(face, 0)->_setMipmapping(true);
			}
		}
	}
	#undef GETLEVEL
}
