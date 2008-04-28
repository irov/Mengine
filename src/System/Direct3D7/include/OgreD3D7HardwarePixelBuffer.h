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
#ifndef __D3D7PIXELBUFFER_H__
#define __D3D7PIXELBUFFER_H__

#include "OgreD3D7Prerequisites.h"
#include "OgreHardwarePixelBuffer.h"

namespace Ogre {

	class D3D7HardwarePixelBuffer: public HardwarePixelBuffer
	{
	protected:
		/// Lock a box
		PixelBox lockImpl(const Image::Box lockBox,  LockOptions options);

		/// Unlock a box
		void unlockImpl(void);
		
		/// A pointer to the Direct3D device.
		IDirect3DDevice7 * mD3DDevice;       

		/// Surface abstracted by this buffer
		LPDIRECTDRAWSURFACE7 mSurface;
		
		// Mipmapping
		bool mDoMipmapGen;

		/// Create (or update) render textures for slices
		void createRenderTextures(bool update);
		/// Destroy render textures for slices
		void destroyRenderTextures();

		// Locked region, needs to be stored as D3D7 Unlock needs the exact pointer used
		// for the lock
		RECT mLockRect;
		RECT *mpRect;

		/// Render targets
		typedef std::vector<RenderTexture*> SliceTRT;
		SliceTRT mSliceTRT;

	public:
		D3D7HardwarePixelBuffer(HardwareBuffer::Usage usage, IDirect3DDevice7 *device);
		
		/// Call this to associate a D3D surface or volume with this pixel buffer
		void bind(LPDIRECTDRAWSURFACE7 surface, PixelFormat format);
		//void bind(IDirect3DDevice7 *dev, LPDIRECTDRAWSURFACE7 mSurface, bool update);
		
		/// @copydoc HardwarePixelBuffer::blit
		void blit(HardwarePixelBuffer *src, const Image::Box &srcBox, const Image::Box &dstBox);
		
		/// @copydoc HardwarePixelBuffer::blitFromMemory
		void blitFromMemory(const PixelBox &src, const Image::Box &dstBox);
		
		/// @copydoc HardwarePixelBuffer::blitToMemory
		void blitToMemory(const Image::Box &srcBox, const PixelBox &dst);
		
		/// Function to set mipmap generation
		void _setMipmapping(bool doMipmapGen);

		/// Function to generate mipmaps
		void _genMipmaps(const RECT &texRect);

		/// Accessor for surface
		LPDIRECTDRAWSURFACE7 getSurface() { return mSurface; }

		/// Get rendertarget for z slice
		RenderTexture *getRenderTarget(size_t zoffset);

		/// Notify TextureBuffer of destruction of render target
		virtual void _clearSliceRTT(size_t zoffset)
		{
			mSliceTRT[zoffset] = 0;
		}

		~D3D7HardwarePixelBuffer();
	};
};
#endif
