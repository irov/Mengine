#	pragma once

#	include "Interface/FileSystemInterface.h"
#	include "Interface/WindowsLayerInterface.h"

#   include "Win32InputStream.h"

#   include "Factory/FactoryStore.h"

namespace Menge
{
	class Win32SharedFile
		: public MappedFileInterface
	{
	public:
		Win32SharedFile();
		~Win32SharedFile();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider );

	public:
		bool initialize( const FilePath & _folder, const FilePath & _fileName ) override;

	public:
        InputStreamInterfacePtr createFileStream() override;
        bool openFileStream( const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size ) override;
        
	private:
        ServiceProviderInterface * m_serviceProvider;
        
		FilePath m_folder;
		FilePath m_fileName;

        typedef FactoryPoolStore<Win32InputStream, 16> TFactoryWin32InputStream;
        TFactoryWin32InputStream m_factoryWin32InputStream;
	};
}	// namespace Menge
