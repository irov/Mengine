#	pragma once

#   include "Interface/ServiceInterface.h"
#	include "Interface/StreamInterface.h"

#	include "Core/ConstString.h"
#	include "Core/FilePath.h"
#	include "Core/String.h"

#	include "Factory/Factorable.h"
#	include "Factory/Factory.h"

namespace Menge
{
#	ifndef MENGINE_FILE_STREAM_BUFFER_SIZE
#	define MENGINE_FILE_STREAM_BUFFER_SIZE 4096
#	endif
    //////////////////////////////////////////////////////////////////////////
	class FileInputStreamInterface
		: public InputStreamInterface
	{
	public:
		virtual bool open( const FilePath & _folder, const FilePath & _fileName, size_t _offset, size_t _size ) = 0;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<FileInputStreamInterface> FileInputStreamInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
	class FileMappedInterface
		: public ServantInterface
	{
	public:
		virtual bool open( const FilePath & _folder, const FilePath & _fileName ) = 0;

	public:
        virtual InputStreamInterfacePtr createFileStream() = 0;
        virtual bool openFileStream( const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, void ** _memory ) = 0;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<FileMappedInterface> FileMappedInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class FileOutputStreamInterface
		: public OutputStreamInterface
	{
	public:
		virtual bool open( const FilePath & _folder, const FilePath & _fileName ) = 0;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<FileOutputStreamInterface> FileOutputStreamInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
    class FileGroupInterface
        : public ServantInterface
    {	
    public:
        virtual bool initialize( const FilePath & _path ) = 0;
        virtual void finalize() = 0;

    public:
		virtual bool isPacked() const = 0;

	public:
        virtual const FilePath & getPath() const = 0;

    public:
        virtual bool existFile( const FilePath & _fileName ) const = 0;

    public:
        virtual InputStreamInterfacePtr createInputFile( const FilePath & _fileName, bool _streaming ) = 0;
        virtual bool openInputFile( const FilePath & _fileName, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming ) = 0;

    public:
        virtual OutputStreamInterfacePtr createOutputFile() = 0;
        virtual bool openOutputFile( const FilePath & _fileName, const OutputStreamInterfacePtr & _stream ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<FileGroupInterface> FileGroupInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
    class FileServiceInterface 
        : public ServiceInterface
    {
        SERVICE_DECLARE( "FileService" )

	public:
		virtual void registerFileGroupFactory( const ConstString & _type, const FactoryPtr & _factory ) = 0;
		virtual void unregisterFileGroupFactory( const ConstString & _type ) = 0;

    public:
        virtual bool mountFileGroup( const ConstString & _fileGroupName, const FilePath & _path, const ConstString & _type ) = 0;
        virtual bool unmountFileGroup( const ConstString & _fileGroupName ) = 0;

    public:
        virtual bool hasFileGroup( const ConstString & _fileGroupName, FileGroupInterfacePtr * _fileGroup ) const = 0;
        virtual FileGroupInterfacePtr getFileGroup( const ConstString & _fileGroupName ) const = 0;

	public:
		virtual bool existFile( const ConstString & _fileGroupName, const FilePath & _fileName, FileGroupInterfacePtr * _fileGroup ) const = 0;

	public:
		virtual InputStreamInterfacePtr openInputFile( const ConstString & _fileGroupName, const FilePath & _fileName, bool _streamable ) = 0;
		virtual OutputStreamInterfacePtr openOutputFile( const ConstString & _fileGroupName, const FilePath & _fileName ) = 0;
	};
    //////////////////////////////////////////////////////////////////////////
#   define FILE_SERVICE( serviceProvider )\
    ((Menge::FileServiceInterface *)SERVICE_GET(serviceProvider, Menge::FileServiceInterface))
}


