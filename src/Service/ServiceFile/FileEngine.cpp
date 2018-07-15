#include "FileEngine.h"

#include "Kernel/Logger.h"

#include "Interface/ServiceInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/FactoryDefault.h"

#include "Kernel/String.h"
#include "Kernel/FilePath.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( FileService, Mengine::FileEngine );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
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
    bool FileEngine::_initializeService()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FileEngine::_finalizeService()
    {
        for( TMapFileSystem::reverse_iterator
            it = m_fileSystemMap.rbegin(),
            it_end = m_fileSystemMap.rend();
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
			LOGGER_ERROR("FileEngine::unregisterFileGroupFactory: not registry factory '%s'"
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
			LOGGER_ERROR("FileEngine::createFileGroup: not registry factory '%s'"
				, _type.c_str() 
				);

			return nullptr;
		}

		const FactoryPtr & factory = it_found->second;

		FileGroupInterfacePtr fileGroup = factory->createObject();

		if( fileGroup == nullptr )
		{
			LOGGER_ERROR("FileEngine::createFileGroup: invalid create file group '%s'"
				, _type.c_str() 
				);

			return nullptr;
		}

		return fileGroup;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileEngine::mountFileGroup( const ConstString & _name, const FileGroupInterfacePtr & _category, const FilePath & _path, const ConstString & _type, FileGroupInterfacePtr * _fileGroup )
	{
		LOGGER_INFO( "FileEngine:mountFileSystem _fileGroupName '%s' _path '%s' _type '%s'"
			, _name.c_str() 
			, _path.c_str()
			, _type.c_str()
			);
        
		TMapFileSystem::iterator it_find = m_fileSystemMap.find( _name );

		if( it_find != m_fileSystemMap.end() )
		{
			LOGGER_ERROR("FileEngine::mountFileSystem FileSystem with name '%s' is already mount\n"
				"Remount would be performed"
				, _name.c_str() 
				);

			return false;
		}

		FileGroupInterfacePtr fileGroup = this->createFileGroup( _type );

		if( fileGroup == nullptr )
		{
			LOGGER_ERROR("FileEngine::mountFileSystem can't create fileGroup '%s' type '%s' for object '%s'"
                , _name.c_str()
                , _type.c_str()
				, _path.c_str() 
				);

			return false;
		}

		if( fileGroup->initialize( _name, _category, _path ) == false )
		{
			LOGGER_ERROR("FileEngine::mountFileSystem can't initialize FileSystem '%s' for object '%s'"
				, _name.c_str()
				, _path.c_str()
				);

			return false;
		}

        if( _name.empty() == true )
        {
            m_defaultFileGroup = fileGroup;
        }

        if( _fileGroup != nullptr )
        {
            *_fileGroup = fileGroup;
        }

		m_fileSystemMap.insert( std::make_pair(_name, fileGroup) );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
    bool FileEngine::unmountFileGroup( const ConstString & _name )
	{
		TMapFileSystem::iterator it_find = m_fileSystemMap.find( _name );
		if( it_find == m_fileSystemMap.end() )
		{
			LOGGER_ERROR("FileEngine::unmountFileGroup '%s' not mount"
				, _name.c_str() 
				);

			return false;
		}

        const FileGroupInterfacePtr & groupInterface = it_find->second;

        groupInterface->finalize();

        if( _name.empty() == true )
        {
            m_defaultFileGroup = nullptr;
        }

		m_fileSystemMap.erase( it_find );

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
	const FileGroupInterfacePtr & FileEngine::getFileGroup( const ConstString & _name ) const
	{
		TMapFileSystem::const_iterator it_find = m_fileSystemMap.find( _name );

		if( it_find == m_fileSystemMap.end() )
		{
			LOGGER_ERROR("FileEngine::getFileGroup '%s' not mount"
				, _name.c_str()
				);

			return FileGroupInterfacePtr::none();
		}

		const FileGroupInterfacePtr & fileGroup = it_find->second;

		return fileGroup;
	}
    //////////////////////////////////////////////////////////////////////////
    const FileGroupInterfacePtr & FileEngine::getDefaultFileGroup() const
    {
        return m_defaultFileGroup;
    }
	//////////////////////////////////////////////////////////////////////////
	InputStreamInterfacePtr FileEngine::openInputFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _fileName, bool _streaming )
	{
		InputStreamInterfacePtr file = _fileGroup->createInputFile( _fileName, _streaming );

		if( file == nullptr )
		{
			LOGGER_ERROR("FileEngine::openInputFile can't create input file '%s:%s'"
                , _fileGroup->getName().c_str()
				, _fileName.c_str()
				);

			return nullptr;
		}

		if( _fileGroup->openInputFile( _fileName, file, 0, 0, _streaming ) == false )
		{
			LOGGER_ERROR("FileEngine::openInputFile can't open input file '%s:%s'"
                , _fileGroup->getName().c_str()
				, _fileName.c_str()
				);

			return nullptr;
		}		

		return file;
	}
	//////////////////////////////////////////////////////////////////////////
	OutputStreamInterfacePtr FileEngine::openOutputFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _fileName )
	{
        OutputStreamInterfacePtr file = _fileGroup->createOutputFile();

		if( file == nullptr )
		{
			LOGGER_ERROR("FileEngine::openOutputFile can't create output file '%s:%s'"
                , _fileGroup->getName().c_str()
                , _fileName.c_str()
				);

			return nullptr;
		}

		if( _fileGroup->openOutputFile( _fileName, file ) == false )
		{
			LOGGER_ERROR("FileEngine::openOutputFile can't open output file '%s:%s'"
                , _fileGroup->getName().c_str()
                , _fileName.c_str()
				);
			
			return nullptr;
		}

		return file;
	}
}
