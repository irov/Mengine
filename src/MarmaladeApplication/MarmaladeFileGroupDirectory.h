#	pragma once

#	include "Interface/FileSystemInterface.h"

#	include "MarmaladeFileInputStream.h"
#	include "MarmaladeFileOutputStream.h"

#   include "Factory/FactoryStore.h"

namespace Menge
{
	class MarmaladeFileGroupDirectory
		: public FileGroupInterface
	{
	public:
		MarmaladeFileGroupDirectory();
		~MarmaladeFileGroupDirectory();
		
	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool initialize( const FilePath & _path ) override;
		void finalize() override;

	public:
		bool isPacked() const override;

	public:
		const FilePath & getPath() const override;

	public:
		InputStreamInterfacePtr createInputFile( const FilePath & _fileName, bool _streaming ) override;
		bool openInputFile( const FilePath & _fileName, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming ) override;

	public:
		OutputStreamInterfacePtr createOutputFile() override;
		bool openOutputFile( const FilePath & _fileName, const OutputStreamInterfacePtr & _stream ) override;
		
    public:
        FileMappedInterfacePtr createMappedFile() override;
		bool openMappedFile( const FilePath & _fileName, const FileMappedInterfacePtr & _stream ) override;

    public:
		bool existFile( const FilePath & _fileName ) const override;
		bool removeFile( const FilePath& _filename ) override;

    public:
        bool existDirectory( const FilePath & _filename ) const override;
		bool createDirectory( const FilePath& _filename ) override;
		bool removeDirectory( const FilePath& _filename ) override;

	private:
        ServiceProviderInterface * m_serviceProvider;

		FilePath m_path;

        typedef FactoryPoolStore<MarmaladeFileInputStream, 8> TFactoryFileInputStream;
        TFactoryFileInputStream m_factoryInputStream;

        typedef FactoryPoolStore<MarmaladeFileOutputStream, 4> TFactoryFileOutputStream;
        TFactoryFileOutputStream m_factoryOutputStream;
	};
}
