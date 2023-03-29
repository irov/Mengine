#include "Metatype.h"

#include "Interface/ServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"

#include "Kernel/UnicodeHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/ExceptionHelper.h"

#include "Config/StdString.h"

#include "Metacache.h"

#include "stdex/memory_reader.h"

namespace Metabuf
{
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, bool & _value, void * _userData )
    {
        MENGINE_UNUSED( _userData );

        uint8_t bool_value;
        ar.readPOD( bool_value );

        _value = !!bool_value;
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, uint8_t & _value, void * _userData )
    {
        MENGINE_UNUSED( _userData );

        ar.readPOD( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, uint16_t & _value, void * _userData )
    {
        MENGINE_UNUSED( _userData );

        ar.readPOD( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, uint32_t & _value, void * _userData )
    {
        MENGINE_UNUSED( _userData );

        ar.readPOD( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, float & _value, void * _userData )
    {
        MENGINE_UNUSED( _userData );

        ar.readPOD( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Mengine::String & _value, void * _userData )
    {
        MENGINE_UNUSED( _userData );

        uint32_t size;
        ar.readSize( size );

        if( size == 0 )
        {
            _value.clear();

            return;
        }

        _value.resize( size );

        ar.readPODs( _value.data(), size );
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Mengine::ConstString & _value, void * _userData )
    {
        Mengine::Metacache * cache = static_cast<Mengine::Metacache *>(_userData);

        uint32_t index;
        ar.readSize( index );

        const Mengine::ConstString & value = cache->strings[index];

        _value = value;
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Mengine::FilePath & _value, void * _userData )
    {
        Mengine::Metacache * cache = static_cast<Mengine::Metacache *>(_userData);

        uint32_t index;
        ar.readSize( index );

        const Mengine::ConstString & value = cache->strings[index];

#if defined(MENGINE_DEBUG)
        const Mengine::Char * test_value = value.c_str();

        const Mengine::Char * s = MENGINE_STRCHR( test_value, '\\' );

        if( s != nullptr )
        {
            LOGGER_ERROR( "read FilePath '%s' has invalid slash"
                , test_value
            );

            MENGINE_THROW_EXCEPTION( "read FilePath '%s' has invalid slash"
                , test_value
            );
            
            return;
        }
#endif

        _value = Mengine::FilePath( value );
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Mengine::Tags & _value, void * _userData )
    {
        uint32_t count;
        ar.readSize( count );

        for( uint32_t i = 0; i != count; ++i )
        {
            Mengine::ConstString tag;
            archive_read( ar, tag, _userData );

            _value.addTag( tag );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Mengine::WChar & _value, void * _userData )
    {
        MENGINE_UNUSED( _userData );

        uint32_t size;
        ar.readSize( size );

        Mengine::Char utf8[8];
        ar.readBuffer( (void *)utf8, size );

        size_t unicodeSize;

        Mengine::WChar unicode[2];
        if( UNICODE_SYSTEM()
            ->utf8ToUnicode( utf8, size, unicode, 2, &unicodeSize ) == false )
        {
            stdex::throw_memory_reader_exception();

            return;
        }

        if( unicodeSize == 0 )
        {
            stdex::throw_memory_reader_exception();

            return;
        }

        _value = unicode[0];
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Mengine::WString & _value, void * _userData )
    {
        static Mengine::String utf8;
        archive_read( ar, utf8, _userData );

        Mengine::Helper::utf8ToUnicode( utf8, &_value );
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Mengine::Color & _value, void * _userData )
    {
        MENGINE_UNUSED( _userData );

        float rgba[4];
        ar.readPODs( rgba, 4 );

        _value.setRGBA( rgba[0], rgba[1], rgba[2], rgba[3] );
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Mengine::Polygon & _value, void * _userData )
    {
        MENGINE_UNUSED( _userData );

        uint32_t count;
        ar.readSize( count );

        if( count % 2 != 0 )
        {
            stdex::throw_memory_reader_exception();

            return;
        }

        for( uint32_t i = 0; i != count; i += 2 )
        {
            mt::vec2f v;
            archive_read( ar, v, _userData );

            _value.append( v );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Mengine::Viewport & _value, void * _userData )
    {
        MENGINE_UNUSED( _userData );

        ar.readPODs( &_value.begin.x, 4 );
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Mengine::Floats & _value, void * _userData )
    {
        MENGINE_UNUSED( _userData );

        uint32_t count;
        ar.readSize( count );

        if( count == 0 )
        {
            _value.clear();

            return;
        }

        _value.resize( count );

        ar.readPODs( _value.data(), count );
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Mengine::Int8s & _value, void * _userData )
    {
        MENGINE_UNUSED( _userData );

        uint32_t count;
        ar.readSize( count );

        if( count == 0 )
        {
            _value.clear();

            return;
        }

        _value.resize( count );

        ar.readPODs( _value.data(), count );
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Mengine::Int16s & _value, void * _userData )
    {
        MENGINE_UNUSED( _userData );

        uint32_t count;
        ar.readSize( count );

        if( count == 0 )
        {
            _value.clear();

            return;
        }

        _value.resize( count );

        ar.readPODs( _value.data(), count );
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Mengine::Int32s & _value, void * _userData )
    {
        MENGINE_UNUSED( _userData );

        uint32_t count;
        ar.readSize( count );

        if( count == 0 )
        {
            _value.clear();

            return;
        }

        _value.resize( count );

        ar.readPODs( _value.data(), count );
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Mengine::UInt8s & _value, void * _userData )
    {
        MENGINE_UNUSED( _userData );

        uint32_t count;
        ar.readSize( count );

        if( count == 0 )
        {
            _value.clear();

            return;
        }

        _value.resize( count );

        ar.readPODs( _value.data(), count );
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Mengine::UInt16s & _value, void * _userData )
    {
        MENGINE_UNUSED( _userData );

        uint32_t count;
        ar.readSize( count );

        if( count == 0 )
        {
            _value.clear();

            return;
        }

        _value.resize( count );

        ar.readPODs( _value.data(), count );
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Mengine::UInt32s & _value, void * _userData )
    {
        MENGINE_UNUSED( _userData );

        uint32_t count;
        ar.readSize( count );

        if( count == 0 )
        {
            _value.clear();

            return;
        }

        _value.resize( count );

        ar.readPODs( _value.data(), count );
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, mt::vec2f & _value, void * _userData )
    {
        MENGINE_UNUSED( _userData );

        ar.readPODs( &_value.x, 2 );
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, mt::vec3f & _value, void * _userData )
    {
        MENGINE_UNUSED( _userData );

        ar.readPODs( &_value.x, 3 );
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, mt::vec4f & _value, void * _userData )
    {
        MENGINE_UNUSED( _userData );

        ar.readPODs( &_value.x, 4 );
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, mt::mat4f & _value, void * _userData )
    {
        MENGINE_UNUSED( _userData );

        ar.readPODs( &_value.v0.x, 16 );
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, mt::box2f & _value, void * _userData )
    {
        MENGINE_UNUSED( _userData );

        ar.readPODs( &_value.minimum.x, 4 );
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, mt::uv4f & _value, void * _userData )
    {
        MENGINE_UNUSED( _userData );

        ar.readPODs( &_value.p0.x, 8 );
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Mengine::EBlendFactor & _value, void * _userData )
    {
        MENGINE_UNUSED( _userData );

        uint32_t tmp_value;
        ar.readPOD( tmp_value );

        _value = (Mengine::EBlendFactor)tmp_value;
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Mengine::EBlendOp & _value, void * _userData )
    {
        MENGINE_UNUSED( _userData );

        uint32_t tmp_value;
        ar.readPOD( tmp_value );

        _value = (Mengine::EBlendOp)tmp_value;
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Mengine::ETextureOp & _value, void * _userData )
    {
        MENGINE_UNUSED( _userData );

        uint32_t tmp_value;
        ar.readPOD( tmp_value );

        _value = (Mengine::ETextureOp)tmp_value;
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Mengine::ETextureArgument & _value, void * _userData )
    {
        MENGINE_UNUSED( _userData );

        uint32_t tmp_value;
        ar.readPOD( tmp_value );

        _value = (Mengine::ETextureArgument)tmp_value;
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Mengine::ETextureAddressMode & _value, void * _userData )
    {
        MENGINE_UNUSED( _userData );

        uint32_t tmp_value;
        ar.readPOD( tmp_value );

        _value = (Mengine::ETextureAddressMode)tmp_value;
    }
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Mengine::EVertexAttributeType & _value, void * _userData )
    {
        MENGINE_UNUSED( _userData );

        uint32_t tmp_value;
        ar.readPOD( tmp_value );

        _value = (Mengine::EVertexAttributeType)tmp_value;
    }
}
