#	include "FileEngine.h"

#	include "Logger/Logger.h"

#   include "Interface/ServiceInterface.h"
#   include "Interface/StringizeInterface.h"
#   include "Interface/MemoryInterface.h"

#	include "Factory/FactoryDefault.h"

#	include "Core/String.h"
#	include "Core/FilePath.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( FileService, Menge::FileEngine );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileEngine::FileEngine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	FileEngine::~FileEngine()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    bool FileEngine::_initialize()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FileEngine::_finalize()
    {
        for( TMapFileSystem::iterator 
            it = m_fileSystemMap.begin(),
            it_end = m_fileSystemMap.end();
            it != it_end;
            ++it )
        {
            const FileGroupInterfacePtr & group = it->second;

            group->finalize();
        }

        m_factoryFileGroups.clear();
        m_fileSystemMap.clear();
    }
	//////////////////////////////////////////////////////////////////////////
	void FileEngine::registerFileGroupFactory( const ConstString & _type, const FactoryPtr & _factory )
	{
		m_factoryFileGroups.insert( std::make_pair(_type, _factory) );
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

		const FactoryPtr & factory = it_found->second;

		FileGroupInterface * fileGroup = factory->createObject();

		if( fileGroup == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::createFileGroup: invalid create file group '%s'"
				, _type.c_str() 
				);

			return nullptr;
		}

		fileGroup->setServiceProvider( m_serviceProvider );

		return fileGroup;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::mountFileGroup( const ConstString & _name, const ConstString & _category, const FilePath & _path, const ConstString & _type )
	{
		LOGGER_INFO(m_serviceProvider)( "FileEngine:mountFileSystem _fileGroupName '%s' _path '%s' _type '%s'"
			, _name.c_str() 
			, _path.c_str()
			, _type.c_str()
			);
        
		TMapFileSystem::iterator it_find = m_fileSystemMap.find( _name );

		if( it_find != m_fileSystemMap.end() )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::mountFileSystem FileSystem with name '%s' is already mount\n"
				"Remount would be performed"
				, _name.c_str() 
				);

			return false;
		}

		FileGroupInterfacePtr fs = this->createFileGroup( _type );

		if( fs == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::mountFileSystem can't create fileGroup '%s' type '%s' for object '%s'"
                , _name.c_str()
                , _type.c_str()
				, _path.c_str() 
				);

			return false;
		}

		if( fs->initialize( _name, _category, _path ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::mountFileSystem can't initialize FileSystem '%s' for object '%s'"
				, _name.c_str()
				, _path.c_str()
				);

			return false;
		}

		m_fileSystemMap.insert( std::make_pair(_name, fs) );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::unmountFileGroup( const ConstString& _name )
	{
		TMapFileSystem::iterator it_find = m_fileSystemMap.find( _name );
		if( it_find == m_fileSystemMap.end() )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::unmountFileGroup '%s' not mount"
				, _name.c_str() 
				);

			return false;
		}

        const FileGroupInterfacePtr & groupInterface = it_find->second;

        groupInterface->finalize();

		m_fileSystemMap.erase( it_find );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::existFile( const ConstString& _name, const FilePath & _fileName, FileGroupInterfacePtr * _fileGroup ) const
	{
		TMapFileSystem::const_iterator it_find = m_fileSystemMap.find( _name );

		if( it_find == m_fileSystemMap.end() )
		{
			return false;
		}
		
		const FileGroupInterfacePtr & fileGroup = it_find->second;

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
    bool FileEngine::hasFileGroup( const ConstString& _name, FileGroupInterfacePtr * _fileGroup ) const
    {
		TMapFileSystem::const_iterator it_find = m_fileSystemMap.find( _name );

		if( it_find == m_fileSystemMap.end() )
		{
			return false;
		}

		if( _fileGroup != nullptr )
		{
			*_fileGroup = it_find->second;
		}

		return true;
    }
	//////////////////////////////////////////////////////////////////////////
	FileGroupInterfacePtr FileEngine::getFileGroup( const ConstString & _name ) const
	{
		TMapFileSystem::const_iterator it_find = m_fileSystemMap.find( _name );

		if( it_find == m_fileSystemMap.end() )
		{
			LOGGER_ERROR( m_serviceProvider )("FileEngine::getFileGroup '%s' not mount"
				, _name.c_str()
				);

			return nullptr;
		}

		const FileGroupInterfacePtr & fileGroup = it_find->second;

		return fileGroup;
	}
	//////////////////////////////////////////////////////////////////////////
	InputStreamInterfacePtr FileEngine::openInputFile( const ConstString& _name, const FilePath & _fileName, bool _streaming )
	{
		FileGroupInterfacePtr group = this->getFileGroup( _name );

		if( group == nullptr )
		{
            LOGGER_ERROR(m_serviceProvider)("FileEngine::openInputFile can't get group '%s'"
                , _name.c_str()
                );

			return nullptr;
		}

		InputStreamInterfacePtr file = group->createInputFile( _fileName, _streaming );

		if( file == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::openInputFile can't create input file '%s'"
				, _name.c_str()
				);

			return nullptr;
		}

		if( group->openInputFile( _fileName, file, 0, 0, _streaming ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::openInputFile can't open input file '%s' '%s'"
				, _name.c_str()
				, _fileName.c_str()
				);

			return nullptr;
		}		

		return file;
	}
	//////////////////////////////////////////////////////////////////////////
	OutputStreamInterfacePtr FileEngine::openOutputFile( const ConstString & _name, const FilePath & _fileName )
	{
        FileGroupInterfacePtr group = this->getFileGroup( _name );

        if( group == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("FileEngine::openOutputFile can't get group '%s'"
                , _name.c_str()
                );

            return nullptr;
        }

        OutputStreamInterfacePtr file = group->createOutputFile();

		if( file == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::openOutputFile can't create output file '%s'"
				, _name.c_str()
				);

			return nullptr;
		}

		if( group->openOutputFile( _fileName, file ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("FileEngine::openOutputFile can't open output file '%s' '%s'"
				, _name.c_str()
				, _fileName.c_str()
				);
			
			return nullptr;
		}

		return file;
	}
}
