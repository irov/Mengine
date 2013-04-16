#	pragma once

namespace Menge
{
	enum EHardwareBufferUsage 
	{
		// Static buffer which the application rarely modifies once created. Modifying 
		// the contents of this buffer will involve a performance hit.
		HBU_STATIC = 1,
		// Indicates the application would like to modify this buffer with the CPU
		// fairly often. 
		// Buffers created with this flag will typically end up in AGP memory rather 
		// than video memory.
		HBU_DYNAMIC = 2,
		// Indicates the application will never read the contents of the buffer back, 
		// it will only ever write data. Locking a buffer with this flag will ALWAYS 
		// return a pointer to new, blank memory rather than the memory associated 
		// with the contents of the buffer; this avoids DMA stalls because you can 
		// write to a new memory area while the previous one is being used. 
		HBU_WRITE_ONLY = 4,
		// Indicates that the application will be refilling the contents
		// of the buffer regularly (not just updating, but generating the
		// contents from scratch), and therefore does not mind if the contents 
		// of the buffer are lost somehow and need to be recreated. This
		// allows and additional level of optimisation on the buffer.
		// This option only really makes sense when combined with 
		// HBU_DYNAMIC_WRITE_ONLY.
		HBU_DISCARDABLE = 8,
		// Combination of HBU_STATIC and HBU_WRITE_ONLY
		HBU_STATIC_WRITE_ONLY = 5, 
		// Combination of HBU_DYNAMIC and HBU_WRITE_ONLY. If you use 
		// this, strongly consider using HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE
		// instead if you update the entire contents of the buffer very 
		// regularly. 
		HBU_DYNAMIC_WRITE_ONLY = 6,
		// Combination of HBU_DYNAMIC, HBU_WRITE_ONLY and HBU_DISCARDABLE
		HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE = 14
	};

	/// Locking options
	enum ELockOptions
	{
		// Normal mode, ie allows read/write and contents are preserved.
		HBL_NORMAL,
		// Discards the <em>entire</em> buffer while locking; this allows optimisation to be 
		// performed because synchronisation issues are relaxed. Only allowed on buffers 
		// created with the HBU_DYNAMIC flag. 
		HBL_DISCARD,
		// Lock the buffer for reading only. Not allowed in buffers which are created with HBU_WRITE_ONLY. 
		// Mandatory on statuc buffers, ie those created without the HBU_DYNAMIC flag. 
		HBL_READ_ONLY,
		// As HBL_NORMAL, except the application guarantees not to overwrite any 
		// region of the buffer which has already been used in this frame, can allow
		// some optimisation on some APIs.
		HBL_NO_OVERWRITE
	};

	struct LockData
	{
		/// The data pointer 
		void *data;
		// Pixel Format
		int format;
		// row pitch
		size_t rowPitch;

		// slice pitch
		size_t slicePitch;

		size_t left, top, right, bottom, front, back;
	};

	class HardwarePixelBufferInterface
	{
	public:
		/// Gets the width of this buffer
		virtual size_t getWidth() const = 0;
		/// Gets the height of this buffer
		virtual size_t getHeight() const = 0;
		/// Gets the depth of this buffer
		virtual size_t getDepth() const = 0;
		/// Gets the native pixel format of this buffer
		virtual int getFormat() const = 0;

		virtual void blit( const HardwarePixelBufferInterface* _src ) = 0; 

		virtual void blitFromMemory( const LockData& _src ) = 0;
		virtual void blitFromMemory( const LockData& _src, const size_t* _box ) = 0;
		//virtual void blitToMemory( const float* _box, const LockData& _dst ) = 0;
	};
}