#include "ResourceJSON.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"

#include "Kernel/MemoryHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

#include "jpp/jpp.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static void* my_jpp_malloc( size_t _size )
    {
        return Helper::allocateMemory( _size, "ResourceJSON" );
    }
    //////////////////////////////////////////////////////////////////////////
    static void my_jpp_free( void* _ptr )
    {
        Helper::freeMemory( _ptr, "ResourceJSON" );
    }
    //////////////////////////////////////////////////////////////////////////
    static void my_jpp_error( int32_t _line, int32_t _column, int32_t _position, const char* _source, const char* _text, void* _ud )
    {
        MENGINE_UNUSED( _ud );

        LOGGER_ERROR( "jpp error: %s\nline: %d\n column: %d\nposition: %d\nsource: %s\n"
            , _text
            , _line
            , _column
            , _position
            , _source
        );
    }
    //////////////////////////////////////////////////////////////////////////
    struct my_json_load_data
    {
        const uint8_t* buffer;
        size_t carriage;
        size_t capacity;
    };
    //////////////////////////////////////////////////////////////////////////
    static size_t my_jpp_load_callback( void* _buffer, size_t _buflen, void* _data )
    {
        my_json_load_data* jd = static_cast<my_json_load_data*>(_data);

        if( _buflen > jd->capacity - jd->carriage )
        {
            _buflen = jd->capacity - jd->carriage;
        }

        if( _buflen > 0 )
        {
            const uint8_t* jd_buffer = jd->buffer + jd->carriage;
            ::memcpy( _buffer, jd_buffer, _buflen );
            jd->carriage += _buflen;

            return _buflen;
        }
        else
        {
            return 0;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceJSON::ResourceJSON()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceJSON::~ResourceJSON()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const jpp::object & ResourceJSON::getJSON() const
    {
        return m_json;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceJSON::_compile()
    {
        const FileGroupInterfacePtr& fileGroup = this->getFileGroup();
        const FilePath& filePath = this->getFilePath();

        InputStreamInterfacePtr stream = FILE_SERVICE()
            ->openInputFile( fileGroup, filePath, false, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, false );

        MemoryInterfacePtr memory = Helper::createMemoryStream( stream, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, false );

        const void* memory_buffer = memory->getBuffer();
        size_t memory_size = memory->getSize();

        my_json_load_data jd;
        jd.buffer = static_cast<const uint8_t*>(memory_buffer);
        jd.carriage = 0;
        jd.capacity = memory_size;

        jpp::object json = jpp::load( &my_jpp_load_callback, &my_jpp_malloc, &my_jpp_free, &my_jpp_error, &jd );

        if( json == jpp::detail::invalid )
        {
            LOGGER_ERROR( "invalid load json '%s'"
                , filePath.c_str()
            );

            return false;
        }

        m_json = json;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceJSON::_release()
    {
        m_json = jpp::detail::invalid;
    }
}