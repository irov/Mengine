#	include "OpenGLRenderVertexBufferES.h"
#	include "OpenGLRenderErrorES.h"

#	include "Core/MemoryAllocator.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	OpenGLRenderVertexBufferES::OpenGLRenderVertexBufferES()
		: m_memory( nullptr )
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
	OpenGLRenderVertexBufferES::~OpenGLRenderVertexBufferES()
	{
		if( m_id != 0 )
		{
			GLCALL( glDeleteBuffers, (1, &m_id) );
		}

		Helper::freeMemory( m_memory );
	}
	//////////////////////////////////////////////////////////////////////////
	bool OpenGLRenderVertexBufferES::initialize( uint32_t _vertexNum, bool _dynamic )
	{
        m_memory = Helper::allocateMemory<RenderVertex2D>( _vertexNum );
		m_vertexNum = _vertexNum;

		m_usage = GL_STATIC_DRAW;

		if( _dynamic == true )
		{
			m_usage = GL_DYNAMIC_DRAW;
		}

		GLuint bufId = 0;
		GLCALL( glGenBuffers, (1, &bufId) );

		GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, bufId) );
		GLCALL( glBufferData, (GL_ARRAY_BUFFER, m_vertexNum * sizeof( RenderVertex2D ), NULL, m_usage) );
		GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, 0) );

		m_id = bufId;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	Pointer OpenGLRenderVertexBufferES::lock( uint32_t _offset, uint32_t _count, EBufferLockFlag _flags )
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
	bool OpenGLRenderVertexBufferES::unlock()
	{
		GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, m_id) );
		GLCALL( glBufferSubData, (GL_ARRAY_BUFFER, m_lockOffset * sizeof( RenderVertex2D ), m_lockCount * sizeof( RenderVertex2D ), m_lockMemory) );
		GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, 0) );

		m_lockOffset = 0;
		m_lockCount = 0;
		m_lockMemory = nullptr;
		m_lockFlags = BLF_LOCK_NONE;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderVertexBufferES::enable()
	{
		GLCALL( glBindBuffer, (GL_ARRAY_BUFFER, m_id) );
	}
}
