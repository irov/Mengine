#	include "MarmaladeRenderVertexBuffer.h"

#	include "MarmaladeRenderError.h"

#	include "Core/MemoryAllocator.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeRenderVertexBuffer::MarmaladeRenderVertexBuffer()
		: m_serviceProvider( nullptr )
		, m_memory( nullptr )
		, m_vertexNum( 0 )
		, m_usage( GL_STATIC_DRAW )
		, m_id( 0 )
		, m_lockOffset(0)
		, m_lockCount(0)
		, m_lockMemory(nullptr)
		, m_lockFlags( BLF_LOCK_NONE )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeRenderVertexBuffer::~MarmaladeRenderVertexBuffer()
	{
		if( m_id != 0 )
		{
			GLCALL( m_serviceProvider, glDeleteBuffers, (1, &m_id) );
		}

		Helper::freeMemory( m_memory );
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeRenderVertexBuffer::initialize( ServiceProviderInterface * _serviceProvider, uint32_t _vertexNum, bool _dynamic )
	{ 
		m_serviceProvider = _serviceProvider;

		m_memory = Helper::allocateMemory<RenderVertex2D>( _vertexNum );
		m_vertexNum = _vertexNum;

		m_usage = GL_STATIC_DRAW;

		if( _dynamic == true )
		{
			m_usage = GL_DYNAMIC_DRAW;
		}

		GLuint bufId = 0;
		GLCALL( m_serviceProvider, glGenBuffers, (1, &bufId) );

		GLCALL( m_serviceProvider, glBindBuffer, (GL_ARRAY_BUFFER, bufId) );
		GLCALL( m_serviceProvider, glBufferData, (GL_ARRAY_BUFFER, m_vertexNum * sizeof( RenderVertex2D ), NULL, m_usage) );
		GLCALL( m_serviceProvider, glBindBuffer, (GL_ARRAY_BUFFER, 0) );

		m_id = bufId;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderVertex2D * MarmaladeRenderVertexBuffer::lock( uint32_t _offset, uint32_t _count, EBufferLockFlag _flags )
	{
		if( m_lockMemory != nullptr )
		{
			return nullptr;
		}

		if( _offset + _count > m_vertexNum )
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
	bool MarmaladeRenderVertexBuffer::unlock()
	{
		GLCALL( m_serviceProvider, glBindBuffer, (GL_ARRAY_BUFFER, m_id) );
		GLCALL( m_serviceProvider, glBufferSubData, (GL_ARRAY_BUFFER, m_lockOffset * sizeof( RenderVertex2D ), m_lockCount * sizeof( RenderVertex2D ), m_lockMemory) );
		GLCALL( m_serviceProvider, glBindBuffer, (GL_ARRAY_BUFFER, 0) );

		m_lockOffset = 0;
		m_lockCount = 0;
		m_lockMemory = nullptr;
		m_lockFlags = BLF_LOCK_NONE;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderVertexBuffer::enable()
	{
		GLCALL( m_serviceProvider, glBindBuffer, (GL_ARRAY_BUFFER, m_id) );
	}
}