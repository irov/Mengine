#	pragma once

//// Precompiler options
//#include "OgrePrerequisites.h"
#	include "HardwareBuffer.h"
//#include "OgreSharedPtr.h"
#include "PixelFormat.h"
//#include "OgreImage.h"

namespace Menge
{
	class RenderTexture;

	// Specialisation of HardwareBuffer for a pixel buffer. The
	// HardwarePixelbuffer abstracts an 1D, 2D or 3D quantity of pixels
	// stored by the rendering API. The buffer can be located on the card
	// or in main memory depending on its usage. One mipmap level of a
	// texture is an example of a HardwarePixelBuffer.
	class HardwarePixelBuffer : public HardwareBuffer
	{
	public:
		/// Should be called by HardwareBufferManager
		HardwarePixelBuffer( std::size_t _width, std::size_t _height, size_t _depth,
			PixelFormat _format, EHardwareBufferUsage _usage, bool _useSystemMemory, bool _useShadowBuffer );
		~HardwarePixelBuffer();

		// make every lock method from HardwareBuffer available.
		using HardwareBuffer::lock;	

		// Lock the buffer for (potentially) reading / writing.
		// @param lockBox Region of the buffer to lock
		// @param options Locking options
		// @returns PixelBox containing the locked region, the pitches and
		// the pixel format
		virtual const PixelBox& lock( const Image::Box& _lockBox, ELockOptions _options );

		/// @copydoc HardwareBuffer::lock
		virtual void* lock( std::size_t _offset, std::size_t _length, ELockOptions _options );

		//Get the current locked region. This is the same value as returned
		//by lock(const Image::Box, LockOptions)
		//@returns PixelBox containing the locked region
		const PixelBox& getCurrentLock();

		/// @copydoc HardwareBuffer::readData
		virtual void readData( std::size_t _offset, std::size_t _length, void* _dest );

		/// @copydoc HardwareBuffer::writeData
		virtual void writeData( std::size_t _offset, std::size_t _length, const void* _source,
			bool _discardWholeBuffer = false );

		// Copies a box from another PixelBuffer to a region of the 
		// this PixelBuffer. 
		// @param dst		Source pixel buffer
		// @param srcBox	Image::Box describing the source region in src
		// @param dstBox	Image::Box describing the destination region in this buffer
		// @remarks The source and destination regions dimensions don't have to match, in which
		// case scaling is done. This scaling is generally done using a bilinear filter in hardware,
		// but it is faster to pass the source image in the right dimensions.
		// @note Only call this function when both  buffers are unlocked. 
		virtual void blit( const HardwarePixelBuffer* _src, const Image::Box& _srcBox, const Image::Box& _dstBox );

		// Convience function that blits the entire source pixel buffer to this buffer. 
		// If source and destination dimensions don't match, scaling is done.
		// @param src		PixelBox containing the source pixels and format in memory
		// @note Only call this function when the buffer is unlocked. 
		void blit( const HardwarePixelBuffer* _src ); 

		// Copies a region from normal memory to a region of this pixelbuffer. The source
		// image can be in any pixel format supported by OGRE, and in any size. 
		// @param src		PixelBox containing the source pixels and format in memory
		// @param dstBox	Image::Box describing the destination region in this buffer
		// @remarks The source and destination regions dimensions don't have to match, in which
		// case scaling is done. This scaling is generally done using a bilinear filter in hardware,
		// but it is faster to pass the source image in the right dimensions.
		// @note Only call this function when the buffer is unlocked. 
		virtual void blitFromMemory( const PixelBox& _src, const Image::Box& _dstBox ) = 0;

		// Convience function that blits a pixelbox from memory to the entire 
		// buffer. The source image is scaled as needed.
		// @param src		PixelBox containing the source pixels and format in memory
		// @note Only call this function when the buffer is unlocked. 
		void blitFromMemory( const PixelBox& _src )
		{
			blitFromMemory( _src, Box( 0, 0, 0, m_width, m_height, m_depth ) );
		}

		// Copies a region of this pixelbuffer to normal memory.
		// @param srcBox	Image::Box describing the source region of this buffer
		// @param dst		PixelBox describing the destination pixels and format in memory
		// @remarks The source and destination regions don't have to match, in which
		// case scaling is done.
		// @note Only call this function when the buffer is unlocked. 
		virtual void blitToMemory( const Image::Box& _srcBox, const PixelBox& _dst ) = 0;

		// Convience function that blits this entire buffer to a pixelbox.
		// The image is scaled as needed.
		// @param src		PixelBox containing the source pixels and format in memory
		// @note Only call this function when the buffer is unlocked. 
		void blitToMemory( const PixelBox& _dst )
		{
			blitToMemory( Box( 0, 0, 0, m_width, m_height, m_depth ), _dst );
		}

		// Get a render target for this PixelBuffer, or a slice of it. The texture this
		// was acquired from must have TU_RENDERTARGET set, otherwise it is possible to
		// render to it and this method will throw an ERR_RENDERSYSTEM exception.
		// @param slice    Which slice
		// @returns A pointer to the render target. This pointer has the lifespan of this
		// PixelBuffer.
		virtual RenderTexture* getRenderTarget( std::size_t slice = 0 );

		/// Gets the width of this buffer
		std::size_t getWidth() const { return m_width; }
		/// Gets the height of this buffer
		std::size_t getHeight() const { return m_height; }
		/// Gets the depth of this buffer
		std::size_t getDepth() const { return m_depth; }
		/// Gets the native pixel format of this buffer
		PixelFormat getFormat() const { return m_format; }

	protected: 
		// Extents
		std::size_t m_width, m_height, m_depth;
		// Pitches (offsets between rows and slices)
		std::size_t m_rowPitch, m_slicePitch;
		// Internal format
		PixelFormat m_format;
		// Currently locked region
		PixelBox m_currentLock;

		/// Internal implementation of lock(), must be overridden in subclasses
		virtual PixelBox lockImpl( const Image::Box _lockBox,  ELockOptions _options ) = 0;

		/// Internal implementation of lock(), do not OVERRIDE or CALL this
		/// for HardwarePixelBuffer implementations, but override the previous method
		virtual void* lockImpl( std::size_t _offset, std::size_t _length, ELockOptions _options );

		/// Internal implementation of unlock(), must be overridden in subclasses
		// virtual void unlockImpl(void) = 0;

		//Notify TextureBuffer of destruction of render target.
		//Called by RenderTexture when destroyed.
		virtual void clearSliceRTT_( std::size_t _zoffset );
		friend class RenderTexture;
	};
}	// namespace Menge

