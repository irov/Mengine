#	include "Memory.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Memory::Memory()
		: m_serviceProvider(nullptr)
		, m_memory(nullptr)
		, m_size(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Memory::~Memory()
	{
		Helper::freeMemory( m_memory );
		m_memory = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	void Memory::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	void * Memory::newMemory( size_t _size )
	{
		unsigned char * new_memory = Helper::reallocateMemory<unsigned char>( m_memory, _size );

		if( new_memory == nullptr )
		{
			m_memory = nullptr;
			m_size = 0;

			return nullptr;
		}

		m_memory = new_memory;
		m_size = _size;

		return m_memory;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Memory::readStream( const InputStreamInterfacePtr & _stream )
	{
		size_t size = _stream->size();

		void * memory = this->newMemory( size );

		if( memory == nullptr )
		{
			return false;
		}

		if( _stream->read( memory, size ) != size )
		{
			return false;
		}

		return true;
	}
}
