#	pragma once

#   include "Interface/FileSystemInterface.h"

#	include "Win32FileInputStream.h"
#	include "Win32FileOutputStream.h"
#	include "Win32FileMapped.h"

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
		InputStreamInterfacePtr createInputFile( const FilePath & _fileName, bool _streaming ) override;
		bool openInputFile( const FilePath & _fileName, const InputStreamInterfacePtr & _file, size_t _offset, size_t _size, bool _streaming ) override;
		
	public:
		MemoryInputPtr openInputFileInMemory( const FilePath & _fileName, size_t _offset, size_t _size ) override;

    public:
		OutputStreamInterfacePtr createOutputFile() override;        
		bool openOutputFile( const FilePath& _fileName, const OutputStreamInterfacePtr & _file ) override;

	public:
		FileMappedInterfacePtr createMappedFile() override;
		bool openMappedFile( const FilePath & _fileName, const FileMappedInterfacePtr & _stream ) override;

    public:
        bool existDirectory( const FilePath& _path ) const override;
		bool createDirectory( const FilePath& _path ) override;
		bool removeDirectory( const FilePath& _path ) override;		

	private:
        bool initializeDirectory_( bool _create );

    protected:
        ServiceProviderInterface * m_serviceProvider;

		FilePath m_path;

		typedef FactoryPoolStore<Win32FileInputStream, 8> TFactoryFileInputStream;
		TFactoryFileInputStream m_factoryInputStream;

		typedef FactoryPoolStore<Win32FileOutputStream, 4> TFactoryFileOutputStream;
		TFactoryFileOutputStream m_factoryOutputStream;

		typedef FactoryPoolStore<Win32FileMapped, 4> TFactoryWin32MappedFile;
		TFactoryWin32MappedFile m_factoryWin32MappedFile;

		typedef FactoryPoolStore<MemoryInput, 32> TFactoryMemoryInput;
		TFactoryMemoryInput m_factoryMemoryInput;
	};
}	// namespace Menge
