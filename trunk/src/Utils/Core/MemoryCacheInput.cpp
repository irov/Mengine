#	include "MemoryCacheInput.h"

#	include "Interface/CacheInterface.h"

#	include <memory.h>
#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MemoryCacheInput::MemoryCacheInput()
		: m_serviceProvider(nullptr)
		, m_bufferId(0)
		, m_data(nullptr)
		, m_pos(nullptr)
		, m_end(nullptr)
		, m_size(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryCacheInput::~MemoryCacheInput()
	{
		if( m_bufferId != 0 )
		{
			CACHE_SERVICE(m_serviceProvider)
				->unlockBuffer( m_bufferId );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void MemoryCacheInput::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	void * MemoryCacheInput::cacheMemory( size_t _size )
	{
		m_size = _size;

		void * memory;
		m_bufferId = CACHE_SERVICE(m_serviceProvider)
			->lockBuffer( m_size, &memory );

		m_data = reinterpret_cast<unsigned char *>(memory);

		m_pos = m_data;
		m_end = m_data + m_size;

        return m_data;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MemoryCacheInput::read( void * _buf, size_t _count )
	{
		size_t cnt = _count;
		// Read over end of memory?
		if( m_pos + cnt > m_end )
		{
			cnt = m_end - m_pos;
		}

		if( cnt == 0 )
		{
			return 0;
		}

		memcpy( _buf, m_pos, cnt );

		m_pos += cnt;

		return cnt;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MemoryCacheInput::seek( size_t _pos )
	{
		if( _pos > m_size )
		{
			_pos = m_size;
		}

		m_pos = m_data + _pos;

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MemoryCacheInput::size() const 
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MemoryCacheInput::tell() const
	{
        size_t distance = m_pos - m_data;

		return distance;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MemoryCacheInput::time( uint64_t & _time ) const
	{
        (void)_time;

		return false;
	}
}	// namespace Menge
