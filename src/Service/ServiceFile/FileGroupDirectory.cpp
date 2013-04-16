#	include "FileGroupDirectory.h"

#   include "Interface/StringizeInterface.h"

#	include "Logger/Logger.h"

#	include "Core/String.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileGroupDirectory::FileGroupDirectory()
		: m_fileSystem(NULL)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	FileGroupDirectory::~FileGroupDirectory()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileGroupDirectory::initialize( ServiceProviderInterface * _serviceProvider, const FilePath & _path, const ConstString & _type, FileSystemInterface * _fileSystem, bool _create )
	{
        m_serviceProvider = _serviceProvider;

		m_path = _path;
        m_type = _type;

        m_fileSystem = _fileSystem;

        if( this->initializeDirectory_( _create ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupDirectory::initializeDirectory_( bool _create )
    {
		if( m_fileSystem->existFile( m_path ) == true )
		{
            return true;
        }
		
        if( _create == false )
		{
			LOGGER_WARNING(m_serviceProvider)( "FileSystemDirectory::initialize failed to open directory %s"
				, m_path.c_str() 
				);
                
			return false;
		}

		LOGGER_WARNING(m_serviceProvider)( "FileSystemDirectory::initialize create directory %s"
			, m_path.c_str() 
			);

		if( m_fileSystem->createFolder( m_path ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "FileSystemDirectory::initialize failed to create directory %s"
				, m_path.c_str() 
				);

			return false;
		}
		
		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void FileGroupDirectory::finalize()
    {
        //Empty
    }
	//////////////////////////////////////////////////////////////////////////
	const FilePath & FileGroupDirectory::getPath() const
	{
		return m_path;
	}
    //////////////////////////////////////////////////////////////////////////
    const ConstString & FileGroupDirectory::getType() const
    {
        return m_type;
    }
	//////////////////////////////////////////////////////////////////////////
	bool FileGroupDirectory::existFile( const FilePath& _filename )
	{
		FilePath fullname = this->makeFullname_( _filename );

		bool exist = m_fileSystem->existFile( fullname );

        return exist;
	}
	//////////////////////////////////////////////////////////////////////////
	InputStreamInterface* FileGroupDirectory::createInputFile()
	{
		FileInputStreamInterface * inputStream = m_fileSystem->createInputStream();

		return inputStream;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileGroupDirectory::openInputFile( const FilePath& _filename, InputStreamInterface* _stream )
	{
        if( _stream == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)( "FileSystemDirectory::openInputFile failed _stream == NULL"
                );

            return false;
        }

        FilePath fullname = this->makeFullname_( _filename );

        FileInputStreamInterface * file = static_cast<FileInputStreamInterface*>(_stream);

		if( file->open( fullname ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "FileSystemDirectory::openInputFile failed open file %s"
                , m_fullnameCache.c_str()
                );

            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	OutputStreamInterface* FileGroupDirectory::createOutputFile()
	{
        FileOutputStreamInterface * outputStream = m_fileSystem->createOutputStream();

		return outputStream;
	}
	//////////////////////////////////////////////////////////////////////////	
	bool FileGroupDirectory::openOutputFile( const FilePath & _filename, OutputStreamInterface* _stream )
	{
        if( _stream == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)( "FileSystemDirectory::openOutputFile failed _stream == NULL"
                );

            return false;
        }

        FilePath fullname = this->makeFullname_( _filename );

        FileOutputStreamInterface * file = static_cast<FileOutputStreamInterface*>(_stream);

        if( file->open( fullname ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "FileSystemDirectory::openOutputFile failed open file %s"
                , m_fullnameCache.c_str()
                );

            return false;
        }
        		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileGroupDirectory::createDirectory( const FilePath & _path )
	{
		FilePath fullname = this->makeFullname_( _path );
	
		if( m_fileSystem->createFolder( fullname ) == false )
        {
            return false;
        }

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileGroupDirectory::removeDirectory( const FilePath & _path )
	{
		FilePath fullname = this->makeFullname_( _path );

		if( m_fileSystem->deleteFolder( fullname ) == false )
        {
            return false;
        }

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileGroupDirectory::removeFile( const FilePath & _filename )
	{
		FilePath fullname = this->makeFullname_( _filename );

		if( m_fileSystem->deleteFile( fullname ) == false )
        {
            return false;
        }

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	FilePath FileGroupDirectory::makeFullname_( const FilePath & _path )
	{
        FilePath fullpath = concatenationFilePath( m_serviceProvider, m_path, _path );

        return fullpath;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
