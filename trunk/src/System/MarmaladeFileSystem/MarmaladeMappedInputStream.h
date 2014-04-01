#	pragma once

#	include "MarmaladeInputStream.h"

#   include "Utils/Core/MemoryInput.h"

#   include "Factory/FactoryStore.h"

namespace Menge
{
	class MarmaladeMappedInputStream
		: public MappedFileInterface
	{
	public:
		MarmaladeMappedInputStream();
		~MarmaladeMappedInputStream();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider );

    public:
        InputStreamInterfacePtr createFileStream() override;
        bool openFileStream( const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size ) override;

    public:
		bool initialize( const FilePath & _folder, const FilePath & _fileName ) override;

    protected:
        ServiceProviderInterface * m_serviceProvider;
        
        typedef FactoryPoolStore<MemoryInput, 16> TFactoryMemoryInput;
        TFactoryMemoryInput m_factoryMemoryInput;               

        MarmaladeInputStream m_inputStream;
	};
}	// namespace Menge
