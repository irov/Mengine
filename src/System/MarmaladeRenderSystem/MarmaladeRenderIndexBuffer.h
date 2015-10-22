#	pragma once

#	include "Interface/RenderSystemInterface.h"

#   include <IwGL.h>

namespace Menge
{
	class MarmaladeRenderIndexBuffer
		: public RenderIndexBufferInterface
	{
	public:
		MarmaladeRenderIndexBuffer();
		~MarmaladeRenderIndexBuffer();

	public:
		bool initialize( ServiceProviderInterface * _serviceProvider, uint32_t _indexNum, bool _dynamic );

	protected:
		RenderIndices * lock( uint32_t _offset, uint32_t _size, EBufferLockFlag _flags ) override;
		bool unlock() override;

	public:
		void enable();

	protected:
		ServiceProviderInterface * m_serviceProvider;

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
	typedef stdex::intrusive_ptr<MarmaladeRenderIndexBuffer> MarmaladeRenderIndexBufferPtr;
}
