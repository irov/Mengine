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
#include "OgreRenderSystem.h"

#include "OgreD3D7HardwarePixelBuffer.h"
#include "OgreStringConverter.h"

namespace Ogre {
//-----------------------------------------------------------------------------  

D3D7HardwarePixelBuffer::D3D7HardwarePixelBuffer(HardwareBuffer::Usage usage, IDirect3DDevice7 *device):
	HardwarePixelBuffer(0, 0, 0, PF_UNKNOWN, usage, false, false),
	mD3DDevice(device), mSurface(0),
	mDoMipmapGen(0)
{
}
D3D7HardwarePixelBuffer::~D3D7HardwarePixelBuffer()
{
	destroyRenderTextures();
}
//-----------------------------------------------------------------------------  
//void D3D7HardwarePixelBuffer::bind(IDirect3DDevice7 *dev, LPDIRECTDRAWSURFACE7 mSurface, bool update)
void D3D7HardwarePixelBuffer::bind(LPDIRECTDRAWSURFACE7 surface, PixelFormat format)
{
	mSurface = surface;
	DDSURFACEDESC2 desc;
	desc.dwSize = sizeof(DDSURFACEDESC2);
	if(surface->GetSurfaceDesc(&desc) != D3D_OK)
		OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Could not get surface information",
		 "D3D7HardwarePixelBuffer::D3D7HardwarePixelBuffer");
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
		createRenderTextures( false );
}
//-----------------------------------------------------------------------------  
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
	hr = mSurface->Lock(mpRect, &lrect, flags, NULL);
	if (FAILED(hr))		
		OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "Surface locking failed",
		 	"D3D7HardwarePixelBuffer::lockImpl");
	
	rval.rowPitch = mRowPitch;
	rval.slicePitch = mSlicePitch;
	rval.data = lrect.lpSurface;
	return rval;
}
//-----------------------------------------------------------------------------  
void D3D7HardwarePixelBuffer::unlockImpl(void)
{
	HRESULT hr;	
	// Crazy DX7 convention: Unlock needs the same rect pointer that Lock got
	hr = mSurface->Unlock(mpRect);	
	if( FAILED( hr ) )
        OGRE_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR, "Error during Unlock operation.", "D3D7HardwarePixelBuffer::unlockImpl" );

	if(mDoMipmapGen)
	{
		// Update mipmaps
		_genMipmaps(mLockRect);
	}
}
//-----------------------------------------------------------------------------  
void D3D7HardwarePixelBuffer::blit(HardwarePixelBuffer *src, const Image::Box &srcBox, const Image::Box &dstBox)
{
	// TODO
	// D3DXLoadTextureFromSurface
	HardwarePixelBuffer::blit(HardwarePixelBufferSharedPtr(src), srcBox, dstBox);
}
//-----------------------------------------------------------------------------  
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
        mD3DDevice,
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
        mD3DDevice,
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

    if( FAILED( hr = mSurface->Blt(
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

	if(mDoMipmapGen)
		_genMipmaps(texRect);
//	OgreUnguard();
}
//-----------------------------------------------------------------------------  
void D3D7HardwarePixelBuffer::blitToMemory(const Image::Box &srcBox, const PixelBox &dst)
{
	OGRE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED,
				"Function not yet implemented",
				"D3D7HardwarePixelBuffer::blitToMemory");
}
//-----------------------------------------------------------------------------  
void D3D7HardwarePixelBuffer::_setMipmapping(bool doMipmapGen)
{
	mDoMipmapGen = doMipmapGen;
}
//-----------------------------------------------------------------------------  
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
	ddsMipLevel = mSurface;
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
			mD3DDevice,
			ddsNextLevel,
			0,
			mSurface,
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

//-----------------------------------------------------------------------------    
RenderTexture *D3D7HardwarePixelBuffer::getRenderTarget(size_t zoffset)
{
	assert(mUsage & TU_RENDERTARGET);
	assert(zoffset < mDepth);
	return mSliceTRT[zoffset];
}
//-----------------------------------------------------------------------------    
void D3D7HardwarePixelBuffer::createRenderTextures(bool update)
{
	if (update)
	{
		assert(mSliceTRT.size() == mDepth);
		for (SliceTRT::const_iterator it = mSliceTRT.begin(); it != mSliceTRT.end(); ++it)
		{
			D3D7RenderTexture *trt = static_cast<D3D7RenderTexture*>(*it);
			trt->rebind(this);
		}
		return;
	}

	destroyRenderTextures();
	if(!mSurface)
	{
		OGRE_EXCEPT( Exception::ERR_RENDERINGAPI_ERROR, 
			"Rendering to 3D slices not supported yet for Direct3D",
			"D3D7HardwarePixelBuffer::createRenderTexture");
	}
	// Create render target for each slice
	mSliceTRT.reserve(mDepth);
	assert(mDepth==1);
	for(size_t zoffset=0; zoffset<mDepth; ++zoffset)
	{
		String name;
		name = "rtt/"+Ogre::StringConverter::toString((size_t)mSurface);

		RenderTexture *trt = new D3D7RenderTexture(name, this);
		mSliceTRT.push_back(trt);
		Root::getSingleton().getRenderSystem()->attachRenderTarget(*trt);
	}
}
//-----------------------------------------------------------------------------    
void D3D7HardwarePixelBuffer::destroyRenderTextures()
{
	if(mSliceTRT.empty())
		return;
	// Delete all render targets that are not yet deleted via _clearSliceRTT
	for(size_t zoffset=0; zoffset<mDepth; ++zoffset)
	{
		if(mSliceTRT[zoffset])
			Root::getSingleton().getRenderSystem()->destroyRenderTarget(mSliceTRT[zoffset]->getName());
	}
}
};
