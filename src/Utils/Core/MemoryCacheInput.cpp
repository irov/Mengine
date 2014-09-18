#	include "MemoryCacheInput.h"

#	include "Interface/CacheInterface.h"

#	include "stdex/memorycopy.h"

#	include <memory.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MemoryCacheInput::MemoryCacheInput()
		: m_serviceProvider(nullptr)
		, m_bufferId(0)
		, m_data(nullptr)
		, m_size(0)
		, m_pos(nullptr)
		, m_end(nullptr)		
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryCacheInput::~MemoryCacheInput()
	{
		this->uncache_();
	}
	//////////////////////////////////////////////////////////////////////////
	void MemoryCacheInput::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	void MemoryCacheInput::uncache_()
	{
		if( m_bufferId != 0 )
		{
			CACHE_SERVICE(m_serviceProvider)
				->unlockBuffer( m_bufferId );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void * MemoryCacheInput::cacheMemory( size_t _size, const char * _doc )
	{
		this->uncache_();

		void * memory;
		size_t bufferId = CACHE_SERVICE(m_serviceProvider)
			->lockBuffer( _size, &memory, _doc );

		if( bufferId == 0 )
		{
			return nullptr;
		}

		m_bufferId = bufferId;

		m_data = reinterpret_cast<unsigned char *>(memory);
		m_size = _size;

		m_pos = m_data;
		m_end = m_data + m_size;

        return m_data;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MemoryCacheInput::read( void * _buf, size_t _count )
	{
		THREAD_GUARD_SCOPE(this, m_serviceProvider, "MemoryCacheInput::read");

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

		stdex::memorycopy( _buf, m_pos, cnt );
		//std::copy( m_pos, m_pos + cnt, (unsigned char *)_buf );

		m_pos += cnt;

		return cnt;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MemoryCacheInput::seek( size_t _pos )
	{
		THREAD_GUARD_SCOPE(this, m_serviceProvider, "MemoryCacheInput::seek");

		if( _pos > m_size )
		{
			_pos = m_size;
		}

		m_pos = m_data + _pos;

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MemoryCacheInput::skip( size_t _pos )
	{
		THREAD_GUARD_SCOPE(this, m_serviceProvider, "MemoryCacheInput::skip");

		if( m_pos + _pos > m_end )
		{
			_pos = 0;
		}

		m_pos += _pos;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MemoryCacheInput::size() const 
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MemoryCacheInput::eof() const
	{
		THREAD_GUARD_SCOPE(this, m_serviceProvider, "MemoryCacheInput::eof");

		return m_pos == m_end;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MemoryCacheInput::tell() const
	{
		THREAD_GUARD_SCOPE(this, m_serviceProvider, "MemoryCacheInput::tell");

        size_t distance = m_pos - m_data;

		return distance;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MemoryCacheInput::time( uint64_t & _time ) const
	{
        (void)_time;

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MemoryCacheInput::memory( void ** _memory, size_t * _size )
	{
		*_memory = m_data;
		*_size = m_size;

		return true;
	}
}	// namespace Menge
