#	include "FileEngine.h"

#	include "Logger/Logger.h"

#   include "Interface/ServiceInterface.h"
#   include "Interface/StringizeInterface.h"

#	include "Factory/FactoryDefault.h"

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
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * FileEngine::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::initialize()
	{		
		//this->registerFileGroupFactory( Helper::stringizeString(m_serviceProvider, "dir"), new FactorableUnique<FactoryDefault<Win32FileGroupDirectory> >() );
		
        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::registerFileGroupFactory( const ConstString & _type, const FactoryPtr & _factory )
	{
		m_factoryFileGroups.insert( _type, _factory );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::unregisterFileGroupFactory( const ConstString & _type )
	{
		TFactoryFileGroups::iterator it_found = m_factoryFileGroups.find( _type );

		if( it_found == m_factoryFileGroups.end() )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::unregisterFileGroupFactory: not registry factory '%s'"
				, _type.c_str() 
				);

			return;
		}

		m_factoryFileGroups.erase( it_found );	
	}
	//////////////////////////////////////////////////////////////////////////
	FileGroupInterfacePtr FileEngine::createFileGroup( const ConstString & _type )
	{
		TFactoryFileGroups::iterator it_found = m_factoryFileGroups.find( _type );

		if( it_found == m_factoryFileGroups.end() )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::createFileGroup: not registry factory '%s'"
				, _type.c_str() 
				);

			return nullptr;
		}

		const FactoryPtr & factory = m_factoryFileGroups.get_value(it_found);

		Factorable * fileGroup = factory->createObject();

		if( fileGroup == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::createFileGroup: invalid create file group '%s'"
				, _type.c_str() 
				);

			return nullptr;
		}

		return fileGroup;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::mountFileGroup( const ConstString& _fileGroupName, const FilePath& _path, const ConstString & _type )
	{
		LOGGER_INFO(m_serviceProvider)( "FileEngine:mountFileSystem _fileGroupName '%s' _path '%s' _type '%s'"
			, _fileGroupName.c_str() 
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

		FileGroupInterfacePtr fs = this->createFileGroup( _type );

		if( fs == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::mountFileSystem can't create fileGroup '%s' type '%s' for object '%s'"
                , _fileGroupName.c_str()
                , _type.c_str()
				, _path.c_str() 
				);

			return false;
		}

		if( fs->initialize( m_serviceProvider, _path ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::mountFileSystem can't initialize FileSystem '%s' for object '%s'"
				, _fileGroupName.c_str()
				, _path.c_str()
				);

			return false;
		}

		//if( fs->existDirectory( _folder ) == false )
		//{
		//	if( _create == false )
		//	{
		//		LOGGER_WARNING(m_serviceProvider)("FileEngine::mountFileSystem failed to open directory %s:%s"
		//			, _path.c_str()
		//			, _folder.c_str() 
		//			);

		//		return false;
		//	}
		//	
		//	LOGGER_WARNING(m_serviceProvider)("FileEngine::mountFileSystem create directory %s:%s"
		//		, _path.c_str()
		//		, _folder.c_str() 
		//		);

		//	if( fs->createDirectory( _folder ) == false )
		//	{
		//		LOGGER_ERROR(m_serviceProvider)("Win32FileGroupDirectory::initialize failed to create directory %s:%s"
		//			, _path.c_str()
		//			, _folder.c_str()
		//			);

		//		return false;
		//	}
		//}

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

		if( group->openInputFile( _fileName, file, 0, 0 ) == false )
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
    MappedFileInterfacePtr FileEngine::openMappedFile( const ConstString & _fileGroupName, const FilePath & _fileName )
    {
		FileGroupInterfacePtr group = this->getFileGroup( _fileGroupName );

		if( group == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::openMappedFile can't get group '%s'"
				, _fileGroupName.c_str()
				);

			return nullptr;
		}

        MappedFileInterfacePtr mappedFile = group->createMappedFile();

        if( mappedFile == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("FileEngine::openMappedFile can't create output file '%s'"
                , _fileName.c_str()
                );

            return nullptr;
        }

        if( group->openMappedFile( _fileName, mappedFile ) == false )
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
