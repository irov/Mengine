#	pragma once

#	include "Interface/ServiceInterface.h"
#	include "Interface/StreamInterface.h"

#   include "Factory/FactorablePtr.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class MemoryCacheBufferInterface
		: public FactorablePtr
	{
	public:
		virtual void * cacheMemory( size_t _size, const char * _doc ) = 0;

		virtual void * getMemory() const = 0;

		template<class T>
		inline T getMemoryT() const
		{
			void * memory = this->getMemory();

			return static_cast<T>(memory);
		}

		virtual size_t getSize() const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<MemoryCacheBufferInterface> MemoryCacheBufferInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class MemoryCacheInputInterface
		: public InputStreamInterface
	{
	public:
		virtual bool cacheMemory( size_t _size, const char * _doc ) = 0;
		virtual void * getMemory( size_t & _size ) const = 0;

		template<class T>
		T getMemoryT( size_t & _size ) const
		{
			void * memory = this->getMemory( _size );

			return static_cast<T>(memory);
		}
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<MemoryCacheInputInterface> MemoryCacheInputInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class MemoryProxyInputInterface
		: public InputStreamInterface
	{
	public:
		virtual void * setMemory( void * _memory, size_t _offset, size_t _size ) = 0;
		virtual void * getMemory( size_t & _size ) const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<MemoryProxyInputInterface> MemoryProxyInputInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class MemoryInputInterface
		: public InputStreamInterface
	{
	public:
		virtual void * newMemory( size_t _size ) = 0;
		virtual void * copyMemory( const void * _source, size_t _size ) = 0;

		virtual void trimMemory( size_t _size ) = 0;

	public:
		virtual const void * getMemory( size_t & _size ) const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<MemoryInputInterface> MemoryInputInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class MemoryInterface
		: public FactorablePtr
	{
	public:
		virtual void * newMemory( size_t _size ) = 0;

	public:
		virtual void * getMemory( size_t & _size ) const = 0;
		virtual size_t getSize() const = 0;

	public:
		template<class T>
		inline T getMemoryT( size_t & _size ) const
		{
			void * memory = this->getMemory( _size );

			return static_cast<T>(memory);
		}
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<MemoryInterface> MemoryInterfacePtr;
}