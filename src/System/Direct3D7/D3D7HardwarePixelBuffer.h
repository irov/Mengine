#	pragma once

#	include "Interface/HardwareBufferInterface.h"

#	include "D3D7Prerequisites.h"
//#include "OgreHardwarePixelBuffer.h"

namespace Menge 
{

	class RenderTexture;

	class D3D7HardwarePixelBuffer: public HardwarePixelBufferInterface
	{
	protected:
		/// Lock a box
		PixelBox lockImpl(const Image::Box lockBox,  ELockOptions options);

		/// Unlock a box
		void unlockImpl(void);
		
		/// A pointer to the Direct3D device.
		IDirect3DDevice7 * m_D3DDevice;       

		/// Surface abstracted by this buffer
		LPDIRECTDRAWSURFACE7 m_surface;
		
		// Mipmapping
		bool m_doMipmapGen;

		/// Create (or update) render textures for slices
		void createRenderTextures( bool _update );
		/// Destroy render textures for slices
		void destroyRenderTextures();

		// Locked region, needs to be stored as D3D7 Unlock needs the exact pointer used
		// for the lock
		RECT mLockRect;
		RECT *mpRect;

		/// Render targets
		typedef std::vector<RenderTexture*> SliceTRT;
		SliceTRT m_sliceTRT;

	public:
		D3D7HardwarePixelBuffer( EHardwareBufferUsage _usage, IDirect3DDevice7* _device );
		
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
		LPDIRECTDRAWSURFACE7 getSurface() { return m_surface; }

		/// Get rendertarget for z slice
		RenderTexture *getRenderTarget(size_t zoffset);

		/// Notify TextureBuffer of destruction of render target
		virtual void _clearSliceRTT(size_t zoffset)
		{
			m_sliceTRT[zoffset] = 0;
		}

		~D3D7HardwarePixelBuffer();
	};
};
