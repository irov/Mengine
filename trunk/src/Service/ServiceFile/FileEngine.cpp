#	include "FileEngine.h"

#	include "Logger/Logger.h"

#   include "Interface/ServiceInterface.h"
#   include "Interface/StringizeInterface.h"

#	include "Factory/FactoryDefault.h"

#	include "FileGroupDirectory.h"
#	include "FileGroupZip.h"
//#	include "FileInput.h"

#	include "Core/String.h"
#	include "Core/FilePath.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( FileService, Menge::FileServiceInterface, Menge::FileEngine );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileEngine::FileEngine()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	FileEngine::~FileEngine()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void FileEngine::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;

        m_factoryFileGroup.setServiceProvider( m_serviceProvider );
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * FileEngine::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::initialize()
	{
		m_factoryFileGroup.registerFactory( Helper::stringizeString(m_serviceProvider, "dir"), new FactorableUnique<FactoryDefault<FileGroupDirectory> >() );
		m_factoryFileGroup.registerFactory( Helper::stringizeString(m_serviceProvider, "zip"), new FactorableUnique<FactoryDefault<FileGroupZip> >() );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::mountFileGroup( const ConstString& _fileGroupName, const FilePath& _folder, const FilePath& _path, const ConstString & _type, bool _create )
	{
		LOGGER_INFO(m_serviceProvider)( "FileEngine:mountFileSystem _fileGroupName '%s' _path '%s':'%s' _type '%s'"
			, _fileGroupName.c_str() 
			, _folder.c_str()
			, _path.c_str()
			, _type.c_str()
			);
        
		TMapFileSystem::iterator it_find = m_fileSystemMap.find( _fileGroupName );
		if( it_find != m_fileSystemMap.end() )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::mountFileSystem FileSystem with name '%s' is already mount\n"
				"Remount would be performed"
				, _fileGroupName.c_str() 
				);

			return false;
		}

		FileGroupInterfacePtr fs = m_factoryFileGroup.createObjectT<FileGroupInterface>( _type );

		if( fs == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::mountFileSystem can't create fileGroup '%s' type '%s' for object '%s':'%s'"
                , _fileGroupName.c_str()
                , _type.c_str()
                , _folder.c_str()
				, _path.c_str() 
				);

			return false;
		}

		if( fs->initialize( m_serviceProvider, _folder, _path, _create ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::mountFileSystem can't initialize FileSystem for object '%s'"
				, _path.c_str() 
				);

			return false;
		}

		m_fileSystemMap.insert( _fileGroupName, fs );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::unmountFileGroup( const ConstString& _fileGroupName )
	{
		TMapFileSystem::iterator it_find = m_fileSystemMap.find( _fileGroupName );
		if( it_find == m_fileSystemMap.end() )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::unmountFileGroup '%s' not mount"
				, _fileGroupName.c_str() 
				);

			return;
		}

        const FileGroupInterfacePtr & groupInterface = m_fileSystemMap.get_value( it_find );

        groupInterface->finalize();


		m_fileSystemMap.erase( it_find );
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::existFile( const ConstString& _fileGroupName, const FilePath & _fileName, FileGroupInterfacePtr * _fileGroup ) const
	{
		TMapFileSystem::const_iterator it_find = m_fileSystemMap.find( _fileGroupName );
		if( it_find == m_fileSystemMap.end() )
		{
			return false;
		}
		
		const FileGroupInterfacePtr & fileGroup = m_fileSystemMap.get_value( it_find );

		if( fileGroup->existFile( _fileName ) == false )
        {
            return false;
        }

		if( _fileGroup != nullptr )
		{
			*_fileGroup = fileGroup;
		}

        return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool FileEngine::hasFileGroup( const ConstString& _fileGroupName, FileGroupInterfacePtr * _fileGroup ) const
    {
		FileGroupInterfacePtr fileGroup;
		bool result = m_fileSystemMap.has_copy( _fileGroupName, fileGroup );

		if( _fileGroup != nullptr )
		{
			*_fileGroup = fileGroup;
		}

		return result;
    }
	//////////////////////////////////////////////////////////////////////////
	FileGroupInterfacePtr FileEngine::getFileGroup( const ConstString & _fileGroupName ) const
	{
		FileGroupInterfacePtr fileGroup;
		if( m_fileSystemMap.has_copy( _fileGroupName, fileGroup ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::getFileGroup '%s' not mount"
				, _fileGroupName.c_str()
				);

			return nullptr;
		}

		return fileGroup;
	}
	//////////////////////////////////////////////////////////////////////////
	InputStreamInterfacePtr FileEngine::openInputFile( const ConstString& _fileGroupName, const FilePath & _fileName )
	{
		FileGroupInterfacePtr group = this->getFileGroup( _fileGroupName );

		if( group == nullptr )
		{
            LOGGER_ERROR(m_serviceProvider)("FileEngine::openInputFile can't get group '%s'"
                , _fileGroupName.c_str()
                );

			return nullptr;
		}

		InputStreamInterfacePtr file = group->createInputFile( _fileName );

		if( file == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::openInputFile can't create input file '%s'"
				, _fileGroupName.c_str()
				);

			return nullptr;
		}

		if( group->openInputFile( _fileName, file ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::openInputFile can't open input file '%s' '%s'"
				, _fileGroupName.c_str()
				, _fileName.c_str()
				);

			return nullptr;
		}		

		return file;
	}
	//////////////////////////////////////////////////////////////////////////
	OutputStreamInterfacePtr FileEngine::openOutputFile( const ConstString & _fileGroupName, const FilePath & _fileName )
	{
        FileGroupInterfacePtr group = this->getFileGroup( _fileGroupName );

        if( group == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("FileEngine::openOutputFile can't get group '%s'"
                , _fileGroupName.c_str()
                );

            return nullptr;
        }

        OutputStreamInterfacePtr file = group->createOutputFile();

		if( file == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::openOutputFile can't create output file '%s'"
				, _fileGroupName.c_str()
				);

			return nullptr;
		}

		if( group->openOutputFile( _fileName, file ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::openOutputFile can't open output file '%s' '%s'"
				, _fileGroupName.c_str()
				, _fileName.c_str()
				);
			
			return nullptr;
		}

		return file;
	}
    //////////////////////////////////////////////////////////////////////////
    MappedFileInterfacePtr FileEngine::createMappedFile( const FilePath & _fileGroupName, const FilePath & _fileName )
    {
        MappedFileInterfacePtr mappedFile = FILE_SYSTEM(m_serviceProvider)
            ->createMappedFile();

        if( mappedFile == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("FileEngine::openMappedFile can't create output file '%s'"
                , _fileName.c_str()
                );

            return nullptr;
        }

        if( mappedFile->initialize( _fileGroupName, _fileName ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("FileEngine::openMappedFile can't open output file '%s'"
                , _fileName.c_str()
                );

            return nullptr;
        }

        return mappedFile;
    }
	//////////////////////////////////////////////////////////////////////////
	MappedFileInterfacePtr FileEngine::createSharedFile( const FilePath & _fileGroupName, const FilePath & _fileName )
	{
		MappedFileInterfacePtr mappedFile = FILE_SYSTEM(m_serviceProvider)
			->createSharedFile();

		if( mappedFile == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::openMappedFile can't create output file '%s'"
				, _fileName.c_str()
				);

			return nullptr;
		}

		if( mappedFile->initialize( _fileGroupName, _fileName ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::openMappedFile can't open output file '%s'"
				, _fileName.c_str()
				);

			return nullptr;
		}

		return mappedFile;
	}
    //////////////////////////////////////////////////////////////////////////
    bool FileEngine::existDirectory( const ConstString& _fileGroupName, const FilePath& _path ) const
    {
        TMapFileSystem::const_iterator it_find = m_fileSystemMap.find( _fileGroupName );
        if( it_find == m_fileSystemMap.end() )
        {
            return false;
        }

        const FileGroupInterfacePtr & fileGroup = m_fileSystemMap.get_value( it_find );

        if( fileGroup->existDirectory( _path ) == false )
        {
            return false;
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::createDirectory( const ConstString& _fileGroupName, const FilePath & _path )
	{
        FileGroupInterfacePtr group = this->getFileGroup( _fileGroupName );

        if( group == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::createDirectory '%s' not mount"
				, _fileGroupName.c_str() 
				);

			return false;
		}

		if( group->createDirectory( _path ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::removeDirectory( const ConstString& _fileGroupName, const FilePath & _path )
	{
        FileGroupInterfacePtr group = this->getFileGroup( _fileGroupName );

		if( group == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::removeDirectory '%s' not mount"
				, _fileGroupName.c_str() 
				);

			return false;
		}

		if( group->removeDirectory( _path ) == false )
        {
            return false;
        }

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::removeFile( const ConstString& _fileGroupName, const FilePath & _fileName )
	{
        FileGroupInterfacePtr group = this->getFileGroup( _fileGroupName );

        if( group == nullptr )
        {
			LOGGER_ERROR(m_serviceProvider)("FileEngine::removeFile '%s' not mount"
				, _fileGroupName.c_str() 
				);

			return false;
		}
		
		if( group->removeFile( _fileName ) == false )
        {
            return false;
        }

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
}
