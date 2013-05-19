#	pragma once

#   include "Interface/ServiceInterface.h"
#	include "Interface/StreamInterface.h"

#	include "Core/ConstString.h"
#	include "Core/FilePath.h"

#	include "Factory/Factorable.h"

namespace Menge
{
	class FileInputStreamInterface
		: public InputStreamInterface
	{
	public:
		virtual bool open( const FilePath & _filename ) = 0;
	};

    typedef IntrusivePtr<FileInputStreamInterface> FileInputStreamInterfacePtr;

	class MappedFileInputStreamInterface
		: public FileInputStreamInterface
	{
	public:
        virtual InputStreamInterfacePtr createInputMemory() = 0;
        virtual void openInputMemory( const InputStreamInterfacePtr & _stream, const FilePath & _filename, size_t _offset, size_t _size ) = 0;
	};

    typedef IntrusivePtr<MappedFileInputStreamInterface> MappedFileInputStreamInterfacePtr;
	
	class FileOutputStreamInterface
		: public OutputStreamInterface
	{
	public:
		virtual bool open( const FilePath & _filename ) = 0;
	};

    typedef IntrusivePtr<FileOutputStreamInterface> FileOutputStreamInterfacePtr;
	
	class FileSystemInterface
        : public ServiceInterface
	{
        SERVICE_DECLARE( "FileSystem" )

	public:
		virtual bool existFile( const FilePath & _filename ) const = 0;
		
    public:
		virtual FileInputStreamInterfacePtr createInputStream() = 0;		
        virtual FileOutputStreamInterfacePtr createOutputStream() = 0;

    public:
        virtual MappedFileInputStreamInterfacePtr createMappedInputStream() = 0;

    public:
		virtual bool deleteFile( const FilePath & _filename ) = 0;
		virtual bool createFolder( const FilePath & _path ) = 0;
		virtual bool deleteFolder( const FilePath & _path ) = 0;
	};

#   define FILE_SYSTEM( serviceProvider )\
    (Menge::Helper::getService<Menge::FileSystemInterface>(serviceProvider))

    class FileGroupInterface
        : public Factorable
    {
    public:
        virtual bool initialize( ServiceProviderInterface * _serviceProvider, const FilePath& _path, const ConstString & _type, bool _create ) = 0;
        virtual void finalize() = 0;

    public:
        virtual const FilePath & getPath() const = 0;
        virtual const ConstString & getType() const = 0;

    public:
        virtual bool existFile( const FilePath& _filename ) = 0;

    public:
        virtual InputStreamInterfacePtr createInputFile() = 0;
        virtual bool openInputFile( const FilePath& _filename, const InputStreamInterfacePtr & _stream ) = 0;

    public:
        virtual OutputStreamInterfacePtr createOutputFile() = 0;
        virtual bool openOutputFile( const FilePath& _filename, const OutputStreamInterfacePtr & _stream ) = 0;

    public:
        virtual bool createDirectory( const FilePath& _path ) = 0;
        virtual bool removeDirectory( const FilePath& _path ) = 0;
        virtual bool removeFile( const FilePath& _filename ) = 0;
    };
	
    class FileServiceInterface 
        : public ServiceInterface
    {
        SERVICE_DECLARE( "FileService" )

    public:
        virtual bool initialize() = 0;

    public:
        virtual bool mountFileGroup( const ConstString& _fileSystemName, const FilePath & _path, const ConstString & _type, bool _create ) = 0;
        virtual void unmountFileGroup( const ConstString& _fileSystemName ) = 0;

    public:
        virtual bool hasFileGroup( const ConstString& _fileSystemName ) const = 0;
        virtual FileGroupInterface * getFileGroup( const ConstString& _fileSystemName ) const = 0;

	public:
		virtual bool existFile( const ConstString& _fileSystemName, const FilePath & _filename, FileGroupInterface ** _group ) const = 0;

		virtual InputStreamInterfacePtr openInputFile( const ConstString& _fileSystemName, const FilePath & _filename ) = 0;
		virtual OutputStreamInterfacePtr openOutputFile( const ConstString& _fileSystemName, const FilePath & _filename ) = 0;

    public:
        virtual MappedFileInputStreamInterfacePtr openMappedInputStream( const FilePath& _filename ) = 0;

    public:
        virtual bool createDirectory( const ConstString& _fileSystemName, const FilePath& _path ) = 0;
        virtual bool removeDirectory( const ConstString& _fileSystemName, const FilePath& _path ) = 0;
        virtual bool removeFile( const ConstString& _fileSystemName, const FilePath& _filename ) = 0;
	};

#   define FILE_SERVICE( serviceProvider )\
    (Menge::Helper::getService<Menge::FileServiceInterface>(serviceProvider))
}


