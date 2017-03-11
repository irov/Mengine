#	pragma once

#   include "Interface/FileSystemInterface.h"

#	include "Win32FileInputStream.h"
#	include "Win32FileOutputStream.h"
#	include "Win32FileMapped.h"

#   include "Factory/Factory.h"

namespace Menge
{
	class Win32FileGroupDirectory
		: public ServantBase<FileGroupInterface>
	{
	public:
		Win32FileGroupDirectory();
		~Win32FileGroupDirectory();

	public:
		bool initialize( const FilePath & _path ) override;
        void finalize() override;

	public:
		bool isPacked() const override;

    public:
		const FilePath & getPath() const override;

	public:
		bool existFile( const FilePath & _fileName ) const override;
		bool removeFile( const FilePath& _fileName ) override;

    public:
		InputStreamInterfacePtr createInputFile( const FilePath & _fileName, bool _streaming ) override;
		bool openInputFile( const FilePath & _fileName, const InputStreamInterfacePtr & _file, size_t _offset, size_t _size, bool _streaming ) override;
		
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
		FilePath m_path;

		FactoryPtr m_factoryInputStream;
        FactoryPtr m_factoryOutputStream;
        FactoryPtr m_factoryWin32MappedFile;
	};
}	// namespace Menge
