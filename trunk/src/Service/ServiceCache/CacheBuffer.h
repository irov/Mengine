#	pragma once

#	include "Interface/CacheInterface.h"

#	include "CacheBufferMemory.h"

namespace Menge
{
	class CacheBuffer
		: public CacheBufferInterface
	{
	public:
		CacheBuffer();
		~CacheBuffer();

	public:
		void setBuffer( CacheBufferMemory * _memory );

	public:
		TBlobject::value_type * getMemory() const override;

	protected:
		void _destroy();

	protected:
		CacheBufferMemory * m_memory;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<CacheBuffer> CacheBufferPtr;
	//////////////////////////////////////////////////////////////////////////
}