#	pragma once

#   include "Interface/ServiceInterface.h"
#	include "Interface/StreamInterface.h"

#	include "Core/ConstString.h"
#	include "Core/FilePath.h"
#	include "Core/String.h"

#	include "Factory/Factorable.h"

namespace Menge
{
	class FileInputStreamInterface
		: public InputStreamInterface
	{
	public:
		virtual bool open( const FilePath & _folder, const FilePath & _fileName ) = 0;
	};

    typedef stdex::intrusive_ptr<FileInputStreamInterface> FileInputStreamInterfacePtr;

	class MappedFileInterface
		: public FactorablePtr
	{
	public:
		virtual bool initialize( const FilePath & _folder, const FilePath & _fileName ) = 0;

	public:
        virtual InputStreamInterfacePtr createFileStream() = 0;
        virtual bool openFileStream( const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size ) = 0;
	};

    typedef stdex::intrusive_ptr<MappedFileInterface> MappedFileInterfacePtr;
	
	class FileOutputStreamInterface
		: public OutputStreamInterface
	{
	public:
		virtual bool open( const FilePath & _folder, const FilePath & _filename ) = 0;
	};

    typedef stdex::intrusive_ptr<FileOutputStreamInterface> FileOutputStreamInterfacePtr;
	
	class FileSystemInterface
        : public ServiceInterface
	{
        SERVICE_DECLARE( "FileSystem" )

	public:
		virtual bool existFile( const FilePath & _folder, const FilePath & _fileName ) const = 0;

	public:
        virtual bool deleteFile( const FilePath & _folder, const FilePath & _filename ) = 0;
		
    public:
		virtual FileInputStreamInterfacePtr createInputStream() = 0;		
        virtual FileOutputStreamInterfacePtr createOutputStream() = 0;

    public:
        virtual MappedFileInterfacePtr createMappedFile() = 0;
		virtual MappedFileInterfacePtr createSharedFile() = 0;

    public:		
        virtual bool existFolder( const FilePath & _folder, const FilePath & _path ) const = 0;
		virtual bool createFolder( const FilePath & _folder, const FilePath & _path ) = 0;
		virtual bool deleteFolder( const FilePath & _folder, const FilePath & _path ) = 0;
	};

#   define FILE_SYSTEM( serviceProvider )\
    (Menge::Helper::getService<Menge::FileSystemInterface>(serviceProvider))

    class FileGroupInterface
        : public FactorablePtr
    {
    public:
        virtual bool initialize( ServiceProviderInterface * _serviceProvider, const FilePath & _folder, const FilePath & _path, bool _create ) = 0;
        virtual void finalize() = 0;

    public:
        virtual const FilePath & getPath() const = 0;

    public:
        virtual bool existFile( const FilePath & _fileName ) const = 0;

    public:
        virtual InputStreamInterfacePtr createInputFile() = 0;
        virtual bool openInputFile( const FilePath & _fileName, const InputStreamInterfacePtr & _stream ) = 0;
			
    public:
        virtual OutputStreamInterfacePtr createOutputFile() = 0;
        virtual bool openOutputFile( const FilePath & _filename, const OutputStreamInterfacePtr & _stream ) = 0;

    public:
        virtual bool existDirectory( const FilePath & _path ) const = 0;
        virtual bool createDirectory( const FilePath & _path ) = 0;
        virtual bool removeDirectory( const FilePath & _path ) = 0;
        virtual bool removeFile( const FilePath & _filename ) = 0;
    };

	typedef stdex::intrusive_ptr<FileGroupInterface> FileGroupInterfacePtr;
	
    class FileServiceInterface 
        : public ServiceInterface
    {
        SERVICE_DECLARE( "FileService" )

    public:
        virtual bool initialize() = 0;

    public:
        virtual bool mountFileGroup( const ConstString & _fileGroupName, const FilePath & _folder, const FilePath & _path, const ConstString & _type, bool _create ) = 0;
        virtual void unmountFileGroup( const ConstString & _fileGroupName ) = 0;

    public:
        virtual bool hasFileGroup( const ConstString & _fileGroupName, FileGroupInterfacePtr * _fileGroup ) const = 0;
        virtual FileGroupInterfacePtr getFileGroup( const ConstString & _fileGroupName ) const = 0;

	public:
		virtual bool existFile( const ConstString & _fileGroupName, const FilePath & _fileName, FileGroupInterfacePtr * _fileGroup ) const = 0;

	public:
		virtual InputStreamInterfacePtr openInputFile( const ConstString & _fileGroupName, const FilePath & _fileName ) = 0;
		virtual OutputStreamInterfacePtr openOutputFile( const ConstString & _fileGroupName, const FilePath & _fileName ) = 0;

    public:
        virtual MappedFileInterfacePtr createMappedFile( const FilePath & _foldername, const FilePath& _fileName ) = 0;
		virtual MappedFileInterfacePtr createSharedFile( const FilePath & _foldername, const FilePath& _fileName ) = 0;

    public:
        virtual bool existDirectory( const ConstString& _fileGroupName, const FilePath& _path ) const = 0;
        virtual bool createDirectory( const ConstString& _fileGroupName, const FilePath& _path ) = 0;
        virtual bool removeDirectory( const ConstString& _fileGroupName, const FilePath& _path ) = 0;
        virtual bool removeFile( const ConstString& _fileGroupName, const FilePath& _filename ) = 0;
	};

#   define FILE_SERVICE( serviceProvider )\
    (Menge::Helper::getService<Menge::FileServiceInterface>(serviceProvider))
}


