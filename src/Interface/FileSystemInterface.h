#	pragma once

#   include "Interface/ServiceInterface.h"
#	include "Interface/LogSystemInterface.h"
#	include "Interface/StreamInterface.h"
#	include "Interface/PluginInterface.h"

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

	class MappedFileInputStreamInterface
		: public FileInputStreamInterface
	{
	public:
        virtual InputStreamInterface * createInputMemory() = 0;
        virtual void openInputMemory( InputStreamInterface * _stream, const FilePath & _filename, size_t _offset, size_t _size ) = 0;
		//virtual void * getMemory() const = 0;
	};
	
	class FileOutputStreamInterface
		: public OutputStreamInterface
	{
	public:
		virtual bool open( const FilePath & _filename ) = 0;
	};
	
	class FileSystemInterface
        : public ServiceInterface
	{
        SERVICE_DECLARE( "FileSystem" )

	public:
		virtual bool existFile( const FilePath & _filename ) const = 0;
		
    public:
		virtual FileInputStreamInterface* createInputStream() = 0;		
        virtual FileOutputStreamInterface* createOutputStream() = 0;

    public:
        virtual MappedFileInputStreamInterface * createMappedInputStream() = 0;

    public:
		virtual bool deleteFile( const FilePath & _filename ) = 0;
		virtual bool createFolder( const FilePath & _path ) = 0;
		virtual bool deleteFolder( const FilePath & _path ) = 0;
	};

#   define FILE_SYSTEM( serviceProvider )\
    (Menge::getService<Menge::FileSystemInterface>(serviceProvider))

    class FileGroupInterface
        : public Factorable
    {
    public:
        virtual bool initialize( ServiceProviderInterface * _serviceProvider, const FilePath& _path, const ConstString & _type, FileSystemInterface * _fileSystem, bool _create ) = 0;
        virtual void finalize() = 0;

    public:
        virtual const FilePath & getPath() const = 0;
        virtual const ConstString & getType() const = 0;

    public:
        virtual bool existFile( const FilePath& _filename ) = 0;

    public:
        virtual InputStreamInterface* createInputFile() = 0;
        virtual bool openInputFile( const FilePath& _filename, InputStreamInterface* _stream ) = 0;

    public:
        virtual OutputStreamInterface* createOutputFile() = 0;
        virtual bool openOutputFile( const FilePath& _filename, OutputStreamInterface* _stream ) = 0;

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
        virtual bool mountFileSystem( const ConstString& _fileSystemName, const FilePath & _path, const ConstString & _type, bool _create ) = 0;
        virtual void unmountFileSystem( const ConstString& _fileSystemName ) = 0;

    public:
        virtual bool hasFileSystem( const ConstString& _fileSystemName ) const = 0;
        virtual FileGroupInterface * getFileSystem( const ConstString& _fileSystemName ) const = 0;

	public:
		virtual bool existFile( const ConstString& _fileSystemName, const FilePath & _filename, FileGroupInterface ** _group ) const = 0;

		virtual InputStreamInterface * openInputFile( const ConstString& _fileSystemName, const FilePath & _filename ) = 0;
		virtual OutputStreamInterface * openOutputFile( const ConstString& _fileSystemName, const FilePath & _filename ) = 0;

    public:
        virtual MappedFileInputStreamInterface * openMappedInputStream( const FilePath& _filename ) = 0;

    public:
        virtual bool createDirectory( const ConstString& _fileSystemName, const FilePath& _path ) = 0;
        virtual bool removeDirectory( const ConstString& _fileSystemName, const FilePath& _path ) = 0;
        virtual bool removeFile( const ConstString& _fileSystemName, const FilePath& _filename ) = 0;
	};

#   define FILE_SERVICE( serviceProvider )\
    (Menge::getService<Menge::FileServiceInterface>(serviceProvider))
}


