#	pragma once

#	include "Interface/RenderSystemInterface.h"
#   include "Interface/MemoryInterface.h"

#   include <IwGL.h>

namespace Menge
{
	class MarmaladeRenderIndexBufferES1
		: public RenderIndexBufferInterface
	{
	public:
		MarmaladeRenderIndexBufferES1();
		~MarmaladeRenderIndexBufferES1();

	public:
		bool initialize( ServiceProviderInterface * _serviceProvider, uint32_t _indexNum, bool _dynamic );

	protected:
		RenderIndices * lock( uint32_t _offset, uint32_t _size, EBufferLockFlag _flags ) override;
		bool unlock() override;

	public:
		void enable( uint32_t _offset, uint32_t _size );

	protected:
		ServiceProviderInterface * m_serviceProvider;

		uint32_t m_indexNum;

		GLenum m_usage;

		GLuint m_id;
        
        MemoryInterfacePtr m_memory;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<MarmaladeRenderIndexBufferES1> MarmaladeRenderIndexBufferPtr;
}
