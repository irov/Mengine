
#	include "HardwareBuffer.h"

#	include <cassert>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	HardwareBuffer::HardwareBuffer( EHardwareBufferUsage _usage, bool _systemMemory, bool _useShadowBuffer )
		: m_usage( _usage )
		, m_isLocked( false )
		, m_systemMemory( _systemMemory )
		, m_useShadowBuffer( _useShadowBuffer )
		, m_shadowBuffer( NULL )
		, m_shadowUpdated( false )
		, m_suppressHardwareUpdate( false ) 
	{
		// If use shadow buffer, upgrade to WRITE_ONLY on hardware side
		if ( _useShadowBuffer && _usage == HBU_DYNAMIC )
		{
			m_usage = HBU_DYNAMIC_WRITE_ONLY;
		}
		else if ( _useShadowBuffer && _usage == HBU_STATIC )
		{
			m_usage = HBU_STATIC_WRITE_ONLY;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	HardwareBuffer::~HardwareBuffer()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void* HardwareBuffer::lock( std::size_t _offset, std::size_t _length, ELockOptions _options )
	{
		assert( !isLocked() && "Cannot lock this buffer, it is already locked!" );
		void* ret;
		if( m_useShadowBuffer )
		{
			if( _options != HBL_READ_ONLY )
			{
				// we have to assume a read / write lock so we use the shadow buffer
				// and tag for sync on unlock()
				m_shadowUpdated = true;
			}

			ret = m_shadowBuffer->lock( _offset, _length, _options );
		}
		else
		{
			// Lock the real buffer if there is no shadow buffer 
			ret = lockImpl( _offset, _length, _options );
			m_isLocked = true;
		}
		m_lockStart = _offset;
		m_lockSize = _length;
		return ret;
	}
	//////////////////////////////////////////////////////////////////////////
	void* HardwareBuffer::lock( ELockOptions _options )
	{
		return this->lock( 0, m_sizeInBytes, _options );
	}
	//////////////////////////////////////////////////////////////////////////
	void HardwareBuffer::unlock()
	{
		assert( isLocked() && "Cannot unlock this buffer, it is not locked!" );

		// If we used the shadow buffer this time...
		if( m_useShadowBuffer && m_shadowBuffer->isLocked() )
		{
			m_shadowBuffer->unlock();
			// Potentially update the 'real' buffer from the shadow buffer
			updateFromShadow_();
		}
		else
		{
			// Otherwise, unlock the real one
			unlockImpl();
			m_isLocked = false;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HardwareBuffer::copyData( HardwareBuffer* _srcBuffer, std::size_t _srcOffset, 
		std::size_t _dstOffset, std::size_t _length, bool _discardWholeBuffer )
	{
		const void* srcData = _srcBuffer->lock( _srcOffset, _length, HBL_READ_ONLY );
		this->writeData( _dstOffset, _length, srcData, _discardWholeBuffer );
		_srcBuffer->unlock();
	}
	//////////////////////////////////////////////////////////////////////////
	void HardwareBuffer::updateFromShadow_()
	{
		if( m_useShadowBuffer && m_shadowUpdated && !m_suppressHardwareUpdate )
		{
			// Do this manually to avoid locking problems
			const void *srcData = m_shadowBuffer->lockImpl(
				m_lockStart, m_lockSize, HBL_READ_ONLY);
			// Lock with discard if the whole buffer was locked, otherwise normal
			ELockOptions lockOpt;
			if ( m_lockStart == 0 && m_lockSize == m_sizeInBytes )
			{
				lockOpt = HBL_DISCARD;
			}
			else
			{
				lockOpt = HBL_NORMAL;
			}

			void *destData = this->lockImpl( m_lockStart, m_lockSize, lockOpt );
			// Copy shadow to real
			memcpy( destData, srcData, m_lockSize );
			this->unlockImpl();
			m_shadowBuffer->unlockImpl();
			m_shadowUpdated = false;
		}
	}
	//////////////////////////////////////////////////////////////////////////		
	std::size_t HardwareBuffer::getSizeInBytes() const
	{ 
		return m_sizeInBytes; 
	}
	//////////////////////////////////////////////////////////////////////////
	EHardwareBufferUsage HardwareBuffer::getUsage() const
	{ 
		return m_usage; 
	}	
	//////////////////////////////////////////////////////////////////////////
	bool HardwareBuffer::isSystemMemory() const
	{ 
		return m_systemMemory; 
	}	
	//////////////////////////////////////////////////////////////////////////
	bool HardwareBuffer::hasShadowBuffer() const
	{ 
		return m_useShadowBuffer;
	}	
	//////////////////////////////////////////////////////////////////////////
	bool HardwareBuffer::isLocked() const 
	{ 
		return m_isLocked || ( m_useShadowBuffer && m_shadowBuffer->isLocked() ); 
	}
	//////////////////////////////////////////////////////////////////////////
	void HardwareBuffer::suppressHardwareUpdate( bool _suppress )
	{
		m_suppressHardwareUpdate = _suppress;
		if ( !_suppress )
		{
			updateFromShadow_();
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge