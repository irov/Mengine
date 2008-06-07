#	pragma once

#	include "Interface/HardwareBufferInterface.h"

#	include "D3D7Prerequisites.h"
//#include "OgreHardwarePixelBuffer.h"

namespace Menge 
{

	class RenderTexture;

	class D3D7HardwarePixelBuffer: public HardwarePixelBufferInterface
	{
	public:
		D3D7HardwarePixelBuffer( EHardwareBufferUsage _usage, IDirect3DDevice7* _device );
		~D3D7HardwarePixelBuffer();
		
		/// Call this to associate a D3D surface or volume with this pixel buffer
		void bind(LPDIRECTDRAWSURFACE7 surface, PixelFormat format);
		//void bind(IDirect3DDevice7 *dev, LPDIRECTDRAWSURFACE7 mSurface, bool update);
		
		
		/// @copydoc HardwarePixelBuffer::blitFromMemory
		//void blitFromMemory(const PixelBox &src, const Image::Box &dstBox);
		
		/// @copydoc HardwarePixelBuffer::blitToMemory
		//void blitToMemory(const Image::Box &srcBox, const PixelBox &dst);
		
		/// Function to set mipmap generation
		void _setMipmapping( bool _doMipmapGen );

		/// Function to generate mipmaps
		void _genMipmaps( const RECT& _texRect );

		/// Accessor for surface
		LPDIRECTDRAWSURFACE7 getSurface() { return m_surface; }

		/// Get rendertarget for z slice
		RenderTexture *getRenderTarget( std::size_t _zoffset );

		/// Notify TextureBuffer of destruction of render target
		virtual void _clearSliceRTT( std::size_t _zoffset )
		{
			m_sliceTRT[_zoffset] = 0;
		}

		std::size_t getWidth() const override;
		std::size_t getHeight() const override;
		std::size_t getDepth() const override;
		int getFormat() const override;

		void blit( const HardwarePixelBufferInterface* _src ) override; 
		
		void blit( HardwarePixelBufferInterface* _src, const std::size_t* _srcBox, const std::size_t* _dstBox );

	protected:
		// Extents
		std::size_t m_width, m_height, m_depth;
		// Pitches (offsets between rows and slices)
		std::size_t m_rowPitch, m_slicePitch;
		// Internal format
		PixelFormat m_format;
		EHardwareBufferUsage m_usage;
		std::size_t m_sizeInBytes;
		/// Lock a box
		//PixelBox lockImpl(const Image::Box lockBox,  ELockOptions options);

		/// Unlock a box
		//void unlockImpl(void);

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

	};
};
