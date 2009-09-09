#	pragma once

#	include "Config/Typedef.h"

#	include "Interface/HardwareBufferInterface.h"

namespace Menge
{
	class HardwareBuffer
	{
	public:
		/// Constructor, to be called by HardwareBufferManager only
		HardwareBuffer( EHardwareBufferUsage _usage, bool _systemMemory, bool _useShadowBuffer );

		virtual ~HardwareBuffer();
		// Lock the buffer for (potentially) reading / writing.
		// @param offset The byte offset from the start of the buffer to lock
		// @param length The size of the area to lock, in bytes
		// @param options Locking options
		// @returns Pointer to the locked memory
		virtual void* lock( std::size_t _offset, std::size_t _length, ELockOptions _options );

		// Lock the entire buffer for (potentially) reading / writing.
		// @param options Locking options
		// @returns Pointer to the locked memory
		void* lock( ELockOptions _options );

		// Releases the lock on this buffer. 
		// @remarks 
		// Locking and unlocking a buffer can, in some rare circumstances such as 
		// switching video modes whilst the buffer is locked, corrupt the 
		// contents of a buffer. This is pretty rare, but if it occurs, 
		// this method will throw an exception, meaning you
		// must re-upload the data.
		// @par
		// Note that using the 'read' and 'write' forms of updating the buffer does not
		// suffer from this problem, so if you want to be 100% sure your
		// data will not be lost, use the 'read' and 'write' forms instead.
		virtual void unlock();

		//Reads data from the buffer and places it in the memory pointed to by pDest.
		//@param offset The byte offset from the start of the buffer to read
		//@param length The size of the area to read, in bytes
		//@param pDest The area of memory in which to place the data, must be large enough to 
		//accommodate the data!
		virtual void readData( std::size_t _offset, std::size_t _length, void* _dest ) = 0;

		// Writes data to the buffer from an area of system memory; note that you must
		// ensure that your buffer is big enough.
		// @param offset The byte offset from the start of the buffer to start writing
		// @param length The size of the data to write to, in bytes
		// @param pSource The source of the data to be written
		// @param discardWholeBuffer If true, this allows the driver to discard the entire buffer when writing,
		// such that DMA stalls can be avoided; use if you can.
		virtual void writeData( std::size_t _offset, std::size_t _length, const void* _source,
			bool _discardWholeBuffer = false ) = 0;

		// Copy data from another buffer into this one.
		// @remarks
		// Note that the source buffer must not be created with the
		// usage HBU_WRITE_ONLY otherwise this will fail. 
		// @param srcBuffer The buffer from which to read the copied data
		// @param srcOffset Offset in the source buffer at which to start reading
		// @param dstOffset Offset in the destination buffer to start writing
		// @param length Length of the data to copy, in bytes.
		// @param discardWholeBuffer If true, will discard the entire contents of this buffer before copying
		virtual void copyData( HardwareBuffer* _srcBuffer, std::size_t _srcOffset, 
			std::size_t _dstOffset, std::size_t _length, bool _discardWholeBuffer = false );

		/// Updates the real buffer from the shadow buffer, if required
		virtual void updateFromShadow_();

		/// Returns the size of this buffer in bytes
		std::size_t getSizeInBytes() const;
		/// Returns the Usage flags with which this buffer was created
		EHardwareBufferUsage getUsage() const;
		/// Returns whether this buffer is held in system memory
		bool isSystemMemory() const;
		/// Returns whether this buffer has a system memory shadow for quicker reading
		bool hasShadowBuffer() const;
		/// Returns whether or not this buffer is currently locked.
		bool isLocked() const;
		/// Pass true to suppress hardware upload of shadow buffer changes
		void suppressHardwareUpdate( bool _suppress );

	protected:
		std::size_t m_sizeInBytes;
		EHardwareBufferUsage m_usage;
		bool m_isLocked;
		std::size_t m_lockStart;
		std::size_t m_lockSize;
		bool m_systemMemory;
		bool m_useShadowBuffer;
		HardwareBuffer* m_shadowBuffer;
		bool m_shadowUpdated;
		bool m_suppressHardwareUpdate;

		/// Internal implementation of lock()
		virtual void* lockImpl( std::size_t _offset, std::size_t _length, ELockOptions _options ) = 0;
		/// Internal implementation of unlock()
		virtual void unlockImpl() = 0;


	};
}	// namespace Menge