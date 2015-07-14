#	pragma once

#	include "Interface/ServiceInterface.h"
#	include "Interface/StreamInterface.h"

#   include "Factory/FactorablePtr.h"

#	include "Core/FilePath.h"
#	include "Core/ConstString.h"

namespace Menge
{
	class MemoryCacheBuffer
		: public FactorablePtr
	{
	public:
		MemoryCacheBuffer();
		~MemoryCacheBuffer();
		
	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider );

    public:
        void * cacheMemory( size_t _size, const char * _doc );
		
		inline void * getMemory() const;

		template<class T>
		inline T getMemoryT() const
		{
			void * memory = this->getMemory();

			return static_cast<T>(memory);
		}

		inline size_t getSize() const;

	protected:
		void uncache_();

	protected:
		ServiceProviderInterface * m_serviceProvider;

		uint32_t m_bufferId;

		unsigned char * m_data;
		size_t m_size;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<MemoryCacheBuffer> MemoryCacheBufferPtr;
	//////////////////////////////////////////////////////////////////////////	
	void * MemoryCacheBuffer::getMemory() const
	{
		return m_data;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MemoryCacheBuffer::getSize() const
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace Helper
	{
		//////////////////////////////////////////////////////////////////////////
		MemoryCacheBufferPtr createMemoryBuffer( ServiceProviderInterface * _serviceProvider, size_t _size, const char * _doc );
		//////////////////////////////////////////////////////////////////////////
		MemoryCacheBufferPtr createMemoryStreamSize( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, size_t _size, const char * _doc );
		//////////////////////////////////////////////////////////////////////////
		MemoryCacheBufferPtr createMemoryStream( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, const char * _doc );
		//////////////////////////////////////////////////////////////////////////
		MemoryCacheBufferPtr createMemoryFile( ServiceProviderInterface * _serviceProvider, const ConstString & _category, const FilePath & _filePath, bool _stream, const char * _doc );
		//////////////////////////////////////////////////////////////////////////
		MemoryCacheBufferPtr createMemoryFileString( ServiceProviderInterface * _serviceProvider, const ConstString & _category, const FilePath & _filePath, bool _stream, const char * _doc );
	}
}	// namespace Menge


