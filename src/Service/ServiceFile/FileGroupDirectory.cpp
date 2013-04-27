#	include "FileGroupDirectory.h"

#   include "Interface/StringizeInterface.h"

#	include "Logger/Logger.h"

#	include "Core/String.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileGroupDirectory::FileGroupDirectory()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	FileGroupDirectory::~FileGroupDirectory()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileGroupDirectory::initialize( ServiceProviderInterface * _serviceProvider, const FilePath & _path, const ConstString & _type, bool _create )
	{
        m_serviceProvider = _serviceProvider;

		m_path = _path;
        m_type = _type;

        if( this->initializeDirectory_( _create ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupDirectory::initializeDirectory_( bool _create )
    {
		if( FILE_SYSTEM(m_serviceProvider)->existFile( m_path ) == true )
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

		if( FILE_SYSTEM(m_serviceProvider)->createFolder( m_path ) == false )
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

		bool exist = FILE_SYSTEM(m_serviceProvider)->existFile( fullname );

        return exist;
	}
	//////////////////////////////////////////////////////////////////////////
	InputStreamInterfacePtr FileGroupDirectory::createInputFile()
	{
		FileInputStreamInterfacePtr inputStream = FILE_SYSTEM(m_serviceProvider)
            ->createInputStream();

		return inputStream;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileGroupDirectory::openInputFile( const FilePath& _filename, const InputStreamInterfacePtr & _stream )
	{
        if( _stream == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)( "FileSystemDirectory::openInputFile failed _stream == NULL"
                );

            return false;
        }

        FilePath fullname = this->makeFullname_( _filename );

        FileInputStreamInterfacePtr file = intrusive_static_cast<FileInputStreamInterfacePtr>(_stream);

		if( file->open( fullname ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "FileSystemDirectory::openInputFile failed open file '%s'"
                , fullname.c_str()
                );

            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	OutputStreamInterfacePtr FileGroupDirectory::createOutputFile()
	{
        FileOutputStreamInterfacePtr outputStream = FILE_SYSTEM(m_serviceProvider)
            ->createOutputStream();

		return outputStream;
	}
	//////////////////////////////////////////////////////////////////////////	
	bool FileGroupDirectory::openOutputFile( const FilePath & _filename, const OutputStreamInterfacePtr & _stream )
	{
        if( _stream == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)( "FileSystemDirectory::openOutputFile failed _stream == NULL"
                );

            return false;
        }

        FilePath fullname = this->makeFullname_( _filename );

        FileOutputStreamInterface * file = intrusive_get<FileOutputStreamInterface>(_stream);

        if( file->open( fullname ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "FileSystemDirectory::openOutputFile failed open file %s"
                , fullname.c_str()
                );

            return false;
        }
        		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileGroupDirectory::createDirectory( const FilePath & _path )
	{
		FilePath fullname = this->makeFullname_( _path );
	
		if( FILE_SYSTEM(m_serviceProvider)->createFolder( fullname ) == false )
        {
            return false;
        }

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileGroupDirectory::removeDirectory( const FilePath & _path )
	{
		FilePath fullname = this->makeFullname_( _path );

		if( FILE_SYSTEM(m_serviceProvider)->deleteFolder( fullname ) == false )
        {
            return false;
        }

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileGroupDirectory::removeFile( const FilePath & _filename )
	{
		FilePath fullname = this->makeFullname_( _filename );

		if( FILE_SYSTEM(m_serviceProvider)->deleteFile( fullname ) == false )
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
