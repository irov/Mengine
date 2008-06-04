
#include "D3D7Texture.h"
//#include "Exception.h"
//#include "Image.h"
//#include "LogManager.h"
//#include "HardwarePixelBuffer.h"

//#include "Root.h"
//#include "RenderSystem.h"

#include "D3D7HardwarePixelBuffer.h"
//#include "StringConverter.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	D3D7HardwarePixelBuffer::D3D7HardwarePixelBuffer( EHardwareBufferUsage _usage, IDirect3DDevice7* _device )
		//: HardwarePixelBuffer(0, 0, 0, PF_UNKNOWN, usage, false, false)
		: m_D3DDevice( _device )
		, m_surface( 0 )
		, m_doMipmapGen( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	D3D7HardwarePixelBuffer::~D3D7HardwarePixelBuffer()
	{
		destroyRenderTextures();
	}
	//////////////////////////////////////////////////////////////////////////  
	//void D3D7HardwarePixelBuffer::bind(IDirect3DDevice7 *dev, LPDIRECTDRAWSURFACE7 mSurface, bool update)
	void D3D7HardwarePixelBuffer::bind( LPDIRECTDRAWSURFACE7 _surface, PixelFormat _format )
	{
		m_surface = surface;
		DDSURFACEDESC2 desc;
		desc.dwSize = sizeof(DDSURFACEDESC2);
		HRESULT hr = surface->GetSurfaceDesc(&desc);
		assert( ( hr == D3D_OK ) && "D3D7HardwarePixelBuffer::bind -> Could not get surface information" );
		mWidth = desc.dwWidth;
		mHeight = desc.dwHeight;
		assert((desc.lPitch%PixelUtil::getNumElemBytes(format))==0);
		mRowPitch = desc.lPitch/PixelUtil::getNumElemBytes(format);
		mDepth = 1;
		mFormat = format;
		//mFormat = PF_A8R8G8B8;

		// Calculate pitch and size
		mSlicePitch = mRowPitch*mHeight;
		mSizeInBytes = mRowPitch*mHeight*PixelUtil::getNumElemBytes(format);

		if(mUsage & TU_RENDERTARGET)
		{
			createRenderTextures( false );
		}
	}
	//////////////////////////////////////////////////////////////////////////  
	PixelBox D3D7HardwarePixelBuffer::lockImpl(const Image::Box lockBox,  LockOptions options)
	{
		assert(lockBox.getDepth() == 1);
		// Set extents and format
		PixelBox rval(lockBox, mFormat);

		// Set locking flags according to options
		DWORD flags = DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT;
		switch(options)
		{
		case HBL_DISCARD:
			flags |= DDLOCK_WRITEONLY;
			break;
		case HBL_READ_ONLY:
			flags |= DDLOCK_READONLY;
			break;
		default: 
			break;
		};
		
		DDSURFACEDESC2 lrect; // Filled in by D3D
		HRESULT hr;

		if (lockBox.left == 0 && lockBox.top == 0 
			&& lockBox.right == mWidth && lockBox.bottom == mHeight)
		
		{
			// Lock whole surface
			mpRect = NULL;
		}
		else
		{
			mLockRect.left = lockBox.left;
			mLockRect.right = lockBox.right;
			mLockRect.top = lockBox.top;
			mLockRect.bottom = lockBox.bottom;
			mpRect = &mLockRect;
		}
		hr = m_surface->Lock(mpRect, &lrect, flags, NULL);
		if (FAILED(hr))		
			OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Surface locking failed",
		 		"D3D7HardwarePixelBuffer::lockImpl");
		
		rval.rowPitch = mRowPitch;
		rval.slicePitch = mSlicePitch;
		rval.data = lrect.lpSurface;
		return rval;
	}
	//////////////////////////////////////////////////////////////////////////  
	void D3D7HardwarePixelBuffer::unlockImpl(void)
	{
		HRESULT hr;	
		// Crazy DX7 convention: Unlock needs the same rect pointer that Lock got
		hr = m_surface->Unlock(mpRect);	
		if( FAILED( hr ) )
			OGRE_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR, "Error during Unlock operation.", "D3D7HardwarePixelBuffer::unlockImpl" );

		if(m_doMipmapGen)
		{
			// Update mipmaps
			_genMipmaps(mLockRect);
		}
	}
	//////////////////////////////////////////////////////////////////////////  
	void D3D7HardwarePixelBuffer::blit(HardwarePixelBuffer *src, const Image::Box &srcBox, const Image::Box &dstBox)
	{
		// TODO
		// D3DXLoadTextureFromSurface
		HardwarePixelBuffer::blit(HardwarePixelBufferSharedPtr(src), srcBox, dstBox);
	}
	//////////////////////////////////////////////////////////////////////////  
	void D3D7HardwarePixelBuffer::blitFromMemory(const PixelBox &src, const Image::Box &dstBox)
	{
		/* No 3D textures support in D3D7 */
		assert(src.getDepth() == 1);
		assert(dstBox.getDepth() == 1);

		/* We need a temporary surface in which to load the image data. */
		LPDIRECTDRAWSURFACE7 pddsTempSurface;
		HRESULT hr;
		D3DX_SURFACEFORMAT surfFmt, texFmt;
		MemoryDataStreamPtr buf;
		PixelBox converted;

		/* Compute the pixel format for the image. */
		if(src.format != mFormat)
		{
			/* Format is not directly supported, or the pixelbox is not consecutive; conversion is needed */
			PixelFormat tempFormat = mFormat;
			buf.bind(new MemoryDataStream(
				PixelUtil::getMemorySize(src.getWidth(), src.getHeight(), src.getDepth(),
											tempFormat)));
			converted = PixelBox(src.getWidth(), src.getHeight(), src.getDepth(), tempFormat, buf->getPtr());
			PixelUtil::bulkPixelConversion(src, converted);
		} 
		else
		{
			converted = src;
		}
		surfFmt = D3DTexture::OgreFormat_to_D3DXFormat(mFormat);
		
		/* Compute the current pixel format of the texture. */
		texFmt = D3DTexture::OgreFormat_to_D3DXFormat(mFormat);
		
		/* We generate the mip-maps by hand. */
		DWORD mipFlag, numMips;
		mipFlag = D3DX_TEXTURE_NOMIPMAP;

		/* Set the width and height. */
		DWORD dwWidth = src.getWidth(), dwHeight = src.getHeight();

		/* Create the temporary surface. */
		if( FAILED( hr = D3DXCreateTexture(
			m_D3DDevice,
			NULL,
			&dwWidth,
			&dwHeight,
			&texFmt,
			NULL,
			&pddsTempSurface,
			&numMips ) ) )
		{
			OGRE_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR, "Error during blit operation.", "D3D7HardwarePixelBuffer::blitFromMemory" );
		}

		/* Load the image into the temporary surface. */
		if( FAILED( hr = D3DXLoadTextureFromMemory(
			m_D3DDevice,
			pddsTempSurface,
			D3DX_DEFAULT,
			converted.data,
			NULL,
			surfFmt,
			converted.rowPitch * PixelUtil::getNumElemBytes(converted.format),
			NULL,
			D3DX_FT_LINEAR ) ) )
		{
			pddsTempSurface->Release();
			OGRE_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR, "Error during D3DXLoadTextureFromMemory operation.", "D3D7HardwarePixelBuffer::blitFromMemory" );
		}
		
		RECT texRect;
		texRect.top = dstBox.top;
		texRect.bottom = dstBox.bottom;
		texRect.left = dstBox.left;
		texRect.right = dstBox.right;

		if( FAILED( hr = m_surface->Blt(
			&texRect,
			pddsTempSurface, 
			NULL,
			DDBLT_WAIT,
			NULL ) ) )
		{
			pddsTempSurface->Release();
			OGRE_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR, "Error during blit operation.", "D3DTexture::blitImage" );
		}

		/* Release the temporary surface. */
		pddsTempSurface->Release();

		if(m_doMipmapGen)
			_genMipmaps(texRect);
	//	OgreUnguard();
	}
	//////////////////////////////////////////////////////////////////////////  
	void D3D7HardwarePixelBuffer::blitToMemory(const Image::Box &srcBox, const PixelBox &dst)
	{
		OGRE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED,
					"Function not yet implemented",
					"D3D7HardwarePixelBuffer::blitToMemory");
	}
	//////////////////////////////////////////////////////////////////////////  
	void D3D7HardwarePixelBuffer::_setMipmapping(bool doMipmapGen)
	{
		m_doMipmapGen = doMipmapGen;
	}
	//////////////////////////////////////////////////////////////////////////  
	void D3D7HardwarePixelBuffer::_genMipmaps(const RECT &texRect)
	{
		/* Load the image in all the mip-maps (if there are any, that is). */
		LPDIRECTDRAWSURFACE7 ddsMipLevel, ddsNextLevel;
		DDSCAPS2 ddsCaps;
		HRESULT mipRes = DD_OK;
		uint mipLevel = 1;

		ZeroMemory(&ddsCaps, sizeof(DDSCAPS2));
		ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;

		/* Get the base level and increae the reference count. */
		ddsMipLevel = m_surface;
		ddsMipLevel->AddRef();
		
		/* While we can get a next level in the mip-map chain. */
		while( ddsMipLevel->GetAttachedSurface( &ddsCaps, &ddsNextLevel ) == DD_OK )
		{
			/* Calculate the destination rect. */
			RECT mipRect = { 
				texRect.left >> mipLevel,
				texRect.top  >> mipLevel,
				texRect.right >> mipLevel,
				texRect.bottom >> mipLevel
			};

			/* Blit using D3DX in order to use bilinear filtering. */
			D3DXLoadTextureFromSurface(
				m_D3DDevice,
				ddsNextLevel,
				0,
				m_surface,
				(RECT*)&texRect,
				(RECT*)&mipRect,
				D3DX_FT_LINEAR );
			
			/* Release the current level and get the next one, incrementing the mip depth. */
			ddsMipLevel->Release();
			ddsMipLevel = ddsNextLevel;
			mipLevel++;
		}

		/* Release the last mip-map level surface. */
		ddsMipLevel->Release();
	}

	//////////////////////////////////////////////////////////////////////////    
	RenderTexture *D3D7HardwarePixelBuffer::getRenderTarget(size_t zoffset)
	{
		assert(mUsage & TU_RENDERTARGET);
		assert(zoffset < mDepth);
		return m_sliceTRT[zoffset];
	}
	//////////////////////////////////////////////////////////////////////////    
	void D3D7HardwarePixelBuffer::createRenderTextures(bool update)
	{
		if (update)
		{
			assert(m_sliceTRT.size() == mDepth);
			for (SliceTRT::const_iterator it = m_sliceTRT.begin(); it != m_sliceTRT.end(); ++it)
			{
				D3D7RenderTexture *trt = static_cast<D3D7RenderTexture*>(*it);
				trt->rebind(this);
			}
			return;
		}

		destroyRenderTextures();
		if(!m_surface)
		{
			OGRE_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR, 
				"Rendering to 3D slices not supported yet for Direct3D",
				"D3D7HardwarePixelBuffer::createRenderTexture");
		}
		// Create render target for each slice
		m_sliceTRT.reserve(mDepth);
		assert(mDepth==1);
		for(size_t zoffset=0; zoffset<mDepth; ++zoffset)
		{
			String name;
			name = "rtt/"+Ogre::StringConverter::toString((size_t)m_surface);

			RenderTexture *trt = new D3D7RenderTexture(name, this);
			m_sliceTRT.push_back(trt);
			Root::getSingleton().getRenderSystem()->attachRenderTarget(*trt);
		}
	}
	//////////////////////////////////////////////////////////////////////////    
	void D3D7HardwarePixelBuffer::destroyRenderTextures()
	{
		if(m_sliceTRT.empty())
			return;
		// Delete all render targets that are not yet deleted via _clearSliceRTT
		for(size_t zoffset=0; zoffset<mDepth; ++zoffset)
		{
			if(m_sliceTRT[zoffset])
				Root::getSingleton().getRenderSystem()->destroyRenderTarget(m_sliceTRT[zoffset]->getName());
		}
	}
	//////////////////////////////////////////////////////////////////////////
};
