#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "OpenGLRenderHeaderES.h"

#   include "Core/ServantBase.h"

namespace Mengine
{
	class OpenGLRenderVertexBufferES
		: public ServantBase<RenderVertexBufferInterface>
	{
	public:
		OpenGLRenderVertexBufferES();
		~OpenGLRenderVertexBufferES();

	public:
		bool initialize( uint32_t _verticesNum, bool _dynamic );

	protected:
		Pointer lock( uint32_t _offset, uint32_t _size, EBufferLockFlag _flags ) override;
		bool unlock() override;

	public:
		void enable();

	protected:
		RenderVertex2D * m_memory;
		uint32_t m_vertexNum;

		GLenum m_usage;

		GLuint m_id;

		uint32_t m_lockOffset;
		uint32_t m_lockCount;
		RenderVertex2D * m_lockMemory;
		EBufferLockFlag m_lockFlags;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<OpenGLRenderVertexBufferES> OpenGLRenderVertexBufferESPtr;
}
