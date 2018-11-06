#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "OpenGLRenderHeaderES.h"

#   include "Core/ServantBase.h"

namespace Mengine
{
	class OpenGLRenderIndexBufferES
		: public ServantBase<RenderIndexBufferInterface>
	{
	public:
		OpenGLRenderIndexBufferES();
		~OpenGLRenderIndexBufferES();

	public:
		bool initialize( uint32_t _indexNum, bool _dynamic );

	protected:
		Pointer lock( uint32_t _offset, uint32_t _size, EBufferLockFlag _flags ) override;
		bool unlock() override;

	public:
		void enable();

	protected:
		RenderIndices * m_memory;
		uint32_t m_indexNum;

		GLenum m_usage;

		GLuint m_id;

		uint32_t m_lockOffset;
		uint32_t m_lockCount;
		RenderIndices * m_lockMemory;
		EBufferLockFlag m_lockFlags;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<OpenGLRenderIndexBufferES> OpenGLRenderIndexBufferESPtr;
}
