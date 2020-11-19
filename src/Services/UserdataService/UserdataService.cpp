#include "UserdataService.h"

#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Stream.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( UserdataService, Mengine::UserdataService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    UserdataService::UserdataService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    UserdataService::~UserdataService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool UserdataService::_initializeService()
    {
        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( archivator );

        m_archivator = archivator;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void UserdataService::_finalizeService()
    {
        m_archivator = nullptr;

        m_datas.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool UserdataService::addUserdata( const ConstString & _name, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MapDatas::const_iterator it_found = m_datas.find( _name );

        if( it_found != m_datas.end() )
        {
            LOGGER_ERROR( "data '%s' already exist"
                , _name.c_str()
            );

            return false;
        }

        UserdataDesc desc;
        desc.fileGroup = _fileGroup;
        desc.path = _filePath;

#if MENGINE_DOCUMENT_ENABLE
        desc.doc = _doc;
#endif

        m_datas.emplace( _name, desc );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UserdataService::removeUserdata( const ConstString & _name )
    {
        MapDatas::iterator it_found = m_datas.find( _name );

        if( it_found == m_datas.end() )
        {
            return false;
        }

        m_datas.erase( it_found );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UserdataService::hasUserdata( const ConstString & _name ) const
    {
        MapDatas::const_iterator it_found = m_datas.find( _name );

        if( it_found == m_datas.end() )
        {
            return false;
        }

        const UserdataDesc & desc = it_found->second;

        if( desc.fileGroup->existFile( desc.path, true ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr UserdataService::loadUserdata( const ConstString & _name, const DocumentPtr & _doc ) const
    {
        MapDatas::const_iterator it_found = m_datas.find( _name );

        if( it_found == m_datas.end() )
        {
            LOGGER_ERROR( "data '%s' not found"
                , _name.c_str()
            );

            return nullptr;
        }

        const UserdataDesc & desc = it_found->second;

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( desc.fileGroup, desc.path, false, false, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "data '%s' invalid open file '%s'"
            , _name.c_str()
            , desc.path.c_str()
        );

        MemoryInterfacePtr binaryBuffer = Helper::loadStreamArchiveData( stream, m_archivator, GET_MAGIC_NUMBER( MAGIC_USER_DATA ), GET_MAGIC_VERSION( MAGIC_USER_DATA ), _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( binaryBuffer, "data '%s' invalid load stream archive '%s'"
            , _name.c_str()
            , desc.path.c_str()
        );

        return binaryBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UserdataService::writeUserdata( const ConstString & _name, const void * _data, size_t _size ) const
    {
        MapDatas::const_iterator it_found = m_datas.find( _name );

        if( it_found == m_datas.end() )
        {
            LOGGER_ERROR( "data '%s' not found"
                , _name.c_str()
            );

            return false;
        }

        const UserdataDesc & desc = it_found->second;

        if( _data == nullptr || _size == 0 )
        {
            LOGGER_ERROR( "data '%s' write empty file '%s'"
                , _name.c_str()
                , desc.path.c_str()
            );

            return false;
        }

        OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( desc.fileGroup, desc.path, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "data '%s' invalid open file '%s'"
            , _name.c_str()
            , desc.path.c_str()
        );

        const void * data_memory = _data;
        size_t data_size = _size;

        if( Helper::writeStreamArchiveData( stream, m_archivator, GET_MAGIC_NUMBER( MAGIC_USER_DATA ), GET_MAGIC_VERSION( MAGIC_USER_DATA ), true, data_memory, data_size, EAC_NORMAL ) == false )
        {
            LOGGER_ERROR( "data '%s' invalid write file '%s'"
                , _name.c_str()
                , desc.path.c_str()
            );

            return false;
        }

        if( desc.fileGroup->closeOutputFile( stream ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}