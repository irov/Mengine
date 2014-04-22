#	pragma once

#   include "Interface/FileSystemInterface.h"

#	include "Win32InputStream.h"
#	include "Win32OutputStream.h"
#	include "Win32MappedFile.h"

namespace Menge
{
	class Win32FileGroupDirectory
		: public FileGroupInterface
	{
	public:
		Win32FileGroupDirectory();
		~Win32FileGroupDirectory();

	public:
		bool initialize( ServiceProviderInterface * _serviceProvider, const FilePath & _path ) override;
        void finalize() override;

    public:
		const FilePath & getPath() const override;

	public:
		bool existFile( const FilePath & _fileName ) const override;
		bool removeFile( const FilePath& _fileName ) override;

    public:
		InputStreamInterfacePtr createInputFile( const FilePath & _fileName ) override;
		bool openInputFile( const FilePath & _fileName, const InputStreamInterfacePtr & _file, size_t _offset, size_t _size ) override;
		
    public:
		OutputStreamInterfacePtr createOutputFile() override;        
		bool openOutputFile( const FilePath& _fileName, const OutputStreamInterfacePtr & _file ) override;

	public:
		MappedFileInterfacePtr createMappedFile() override;
		bool openMappedFile( const FilePath & _fileName, const MappedFileInterfacePtr & _stream ) override;

    public:
        bool existDirectory( const FilePath& _path ) const override;
		bool createDirectory( const FilePath& _path ) override;
		bool removeDirectory( const FilePath& _path ) override;		

	private:
        bool initializeDirectory_( bool _create );

    protected:
        ServiceProviderInterface * m_serviceProvider;

		FilePath m_path;

		typedef FactoryPoolStore<Win32InputStream, 8> TFactoryFileInputStream;
		TFactoryFileInputStream m_factoryInputStream;

		typedef FactoryPoolStore<Win32OutputStream, 4> TFactoryFileOutputStream;
		TFactoryFileOutputStream m_factoryOutputStream;

		typedef FactoryPoolStore<Win32MappedFile, 4> TFactoryWin32MappedFile;
		TFactoryWin32MappedFile m_factoryWin32MappedFile;
	};
}	// namespace Menge
