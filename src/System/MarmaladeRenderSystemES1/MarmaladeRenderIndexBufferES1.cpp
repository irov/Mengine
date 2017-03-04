#	include "MarmaladeRenderIndexBufferES1.h"

#	include "MarmaladeRenderErrorES1.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeRenderIndexBufferES1::MarmaladeRenderIndexBufferES1()
		: m_serviceProvider( nullptr )
		, m_indexNum( 0 )
		, m_usage( GL_STATIC_DRAW )
		, m_id( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeRenderIndexBufferES1::~MarmaladeRenderIndexBufferES1()
	{
		if( m_id != 0 )
		{
			GLCALL( m_serviceProvider, glDeleteBuffers, (1, &m_id) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeRenderIndexBufferES1::initialize( ServiceProviderInterface * _serviceProvider, uint32_t _indexNum, bool _dynamic )
	{
		m_serviceProvider = _serviceProvider;
        
        m_indexNum = _indexNum;

        MemoryInterfacePtr memory = MEMORY_SERVICE(m_serviceProvider)
            ->createMemory();
        
        size_t indices_size = _indexNum * sizeof( RenderIndices );
        memory->newMemory(indices_size);
        
        m_memory = memory;
        
		m_usage = GL_STATIC_DRAW;

		if( _dynamic == true )
		{
			m_usage = GL_DYNAMIC_DRAW;
		}

		GLuint bufId = 0;
		GLCALL( m_serviceProvider, glGenBuffers, (1, &bufId) );
        
		m_id = bufId;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderIndices * MarmaladeRenderIndexBufferES1::lock( uint32_t _offset, uint32_t _count, EBufferLockFlag _flags )
	{
		if( _offset + _count > m_indexNum )
		{
			return nullptr;
		}
        
        RenderIndices * memory_buffer = m_memory->getMemory();

		RenderIndices * lock_buffer = memory_buffer + _offset;

		return lock_buffer;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeRenderIndexBufferES1::unlock()
    {
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderIndexBufferES1::enable( uint32_t _offset, uint32_t _size )
	{
        RenderIndices * memory_buffer = m_memory->getMemory();
        RenderIndices * memory_buffer_offset = memory_buffer + _offset;
        
		GLCALL( m_serviceProvider, glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, m_id) );
        GLCALL( m_serviceProvider, glBufferData, (GL_ELEMENT_ARRAY_BUFFER, _size * sizeof( RenderIndices ), (const GLvoid *)memory_buffer_offset, GL_STATIC_DRAW) );
	}
}
