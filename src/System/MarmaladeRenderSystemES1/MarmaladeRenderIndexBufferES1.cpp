#	include "MarmaladeRenderIndexBufferES1.h"

#	include "MarmaladeRenderErrorES1.h"

#	include "Core/MemoryAllocator.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeRenderIndexBufferES1::MarmaladeRenderIndexBufferES1()
		: m_serviceProvider( nullptr )
		, m_memory( nullptr )
		, m_indexNum( 0 )
		, m_usage( GL_STATIC_DRAW )
		, m_id( 0 )
		, m_lockOffset( 0 )
		, m_lockCount( 0 )
		, m_lockMemory( nullptr )
		, m_lockFlags( BLF_LOCK_NONE )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeRenderIndexBufferES1::~MarmaladeRenderIndexBufferES1()
	{
		if( m_id != 0 )
		{
			GLCALL( m_serviceProvider, glDeleteBuffers, (1, &m_id) );
		}

		Helper::freeMemory( m_memory );
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeRenderIndexBufferES1::initialize( ServiceProviderInterface * _serviceProvider, uint32_t _indexNum, bool _dynamic )
	{
		m_serviceProvider = _serviceProvider;

		m_memory = Helper::allocateMemory<RenderIndices>( _indexNum );
		m_indexNum = _indexNum;

		m_usage = GL_STATIC_DRAW;

		if( _dynamic == true )
		{
			m_usage = GL_DYNAMIC_DRAW;
		}

		GLuint bufId = 0;
		GLCALL( m_serviceProvider, glGenBuffers, (1, &bufId) );

		GLCALL( m_serviceProvider, glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, bufId) );
		GLCALL( m_serviceProvider, glBufferData, (GL_ELEMENT_ARRAY_BUFFER, m_indexNum * sizeof( RenderIndices ), NULL, m_usage) );
		GLCALL( m_serviceProvider, glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, 0) );

		m_id = bufId;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderIndices * MarmaladeRenderIndexBufferES1::lock( uint32_t _offset, uint32_t _count, EBufferLockFlag _flags )
	{
		if( m_lockMemory != nullptr )
		{
			return nullptr;
		}

		if( _offset + _count > m_indexNum )
		{
			return nullptr;
		}

		m_lockOffset = _offset;
		m_lockCount = _count;
		m_lockMemory = m_memory + m_lockOffset;
		m_lockFlags = _flags;

		return m_lockMemory;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeRenderIndexBufferES1::unlock()
	{
		GLCALL( m_serviceProvider, glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, m_id) );
		GLCALL( m_serviceProvider, glBufferSubData, (GL_ELEMENT_ARRAY_BUFFER, m_lockOffset * sizeof( RenderIndices ), m_lockCount * sizeof( RenderIndices ), m_lockMemory) );
		GLCALL( m_serviceProvider, glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, 0) );

		m_lockOffset = 0;
		m_lockCount = 0;
		m_lockMemory = nullptr;
		m_lockFlags = BLF_LOCK_NONE;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderIndexBufferES1::enable()
	{
		GLCALL( m_serviceProvider, glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, m_id) );
	}
}