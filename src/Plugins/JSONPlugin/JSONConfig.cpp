#include "JSONConfig.h"

#include "Interface/MemoryServiceInterface.h"
#include "Interface/SecureServiceInterface.h"

#include "Kernel/Exception.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionJSONInvalid.h"
#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/Logger.h"
#include "Kernel/Ravingcode.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/DocumentableHelper.h"
#include "Kernel/ArrayString.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/JSONHelper.h"

#include "Config/Typeinfo.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////        
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static bool getJSONValueT( const jpp::object & _j, const Char * _section, const Char * _key, T * const _value )
        {
            jpp::object j_section;
            if( _j.exist( _section, &j_section ) == false )
            {
                return false;
            }

            jpp::object j_value;
            if( j_section.exist( _key, &j_value ) == false )
            {
                return false;
            }

            MENGINE_ASSERTION_FATAL( jpp::check_object_internal()(j_value.ptr(), static_cast<T *>(nullptr)) == true, "Storage section '%s' key '%s' invalid type '%s' cast '%s'"
                , _section
                , _key
                , jpp::get_jpp_type_string( _j )
                , MENGINE_TYPEINFO_NAME( T )
            );

            *_value = j_value;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static void getJSONValuesT( const jpp::object & _j, const Char * _section, const Char * _key, T * const _values )
        {
            typedef typename T::value_type value_type;

            jpp::object j_section;
            if( _j.exist( _section, &j_section ) == false )
            {
                return;
            }

            jpp::object j_value;
            if( j_section.exist( _key, &j_value ) == false )
            {
                return;
            }

            MENGINE_ASSERTION_FATAL( j_value.is_type_array() == true, "Store section '%s' key '%s' invalid type '%s' is not array"
                , _section
                , _key
                , jpp::get_jpp_type_string( j_value )
            );

            for( const jpp::object & j_element : jpp::array( j_value ) )
            {
                MENGINE_ASSERTION_FATAL( jpp::check_object_internal()(j_element.ptr(), static_cast<typename T::value_type *>(nullptr)) == true, "Storage section '%s' key '%s' invalid type '%s' cast '%s'"
                    , _section
                    , _key
                    , jpp::get_jpp_type_string( j_element )
                    , MENGINE_TYPEINFO_NAME( value_type )
                );

                value_type element = j_element;

                _values->emplace_back( element );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static void setJSONValueT( jpp::object & _j, const Char * _section, const Char * _key, T _value )
        {
            jpp::object j_section = _j.get_or_create( _section, []()
            {
                return jpp::make_object();
            } );

            j_section.set( _key, _value );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static bool hasJSONValueT2( const jpp::object & _j, const Char * _prefix, const Tags & _platform, const Char * _section, const Char * _key, T * const _value )
        {
            ArrayString<128> platform_section;
            platform_section.append( _prefix );
            platform_section.append( _section );

            const VectorConstString & tags = _platform.getValues();

            for( const ConstString & tag : tags )
            {
                platform_section.append( '-' );
                platform_section.append( tag );

                if( Detail::getJSONValueT( _j, platform_section.c_str(), _key, _value ) == true )
                {
                    return true;
                }
            }

            ArrayString<128> section;
            section.append( _prefix );
            section.append( _section );

            if( Detail::getJSONValueT( _j, section.c_str(), _key, _value ) == true )
            {
                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static bool hasJSONValueT( const jpp::object & _json, const Tags & _platform, const Char * _section, const Char * _key, const T & _default, T * const _value )
        {
#if defined(MENGINE_BUILD_PUBLISH)
            if( Detail::hasJSONValueT2( _json, "Publish-", _platform, _section, _key, _value ) == true )
            {
                return true;
            }
#endif

            if( Detail::hasJSONValueT2( _json, MENGINE_MASTER_RELEASE_VALUE( "Alpha-", "Develop-" ), _platform, _section, _key, _value ) == true )
            {
                return true;
            }

            if( Detail::hasJSONValueT2( _json, "", _platform, _section, _key, _value ) == true )
            {
                return true;
            }

            *_value = _default;

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static void calcJSONValuesT2( const jpp::object & _json, const Char * _prefix, const Tags & _platform, const Char * _section, const Char * _key, T * const _value )
        {
            ArrayString<128> platform_section;
            platform_section.append( _prefix );
            platform_section.append( _section );

            const VectorConstString & tags = _platform.getValues();

            for( const ConstString & tag : tags )
            {
                platform_section.append( '-' );
                platform_section.append( tag );

                Detail::getJSONValuesT( _json, platform_section.c_str(), _key, _value );
            }

            ArrayString<128> section;
            section.append( _prefix );
            section.append( _section );

            Detail::getJSONValuesT( _json, section.c_str(), _key, _value );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static void calcJSONValuesT( const jpp::object & _json, const Tags & _platform, const Char * _section, const Char * _key, T * const _value )
        {
#if defined(MENGINE_BUILD_PUBLISH)
            Detail::calcJSONValuesT2( _json, "Publish-", _platform, _section, _key, _value );
#endif

            Detail::calcJSONValuesT2( _json, MENGINE_MASTER_RELEASE_VALUE( "Alpha-", "Develop-" ), _platform, _section, _key, _value );

            Detail::calcJSONValuesT2( _json, "", _platform, _section, _key, _value );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    JSONConfig::JSONConfig()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONConfig::~JSONConfig()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONConfig::setPlatformTags( const Tags & _platformTags )
    {
        m_platformTags = _platformTags;
    }
    //////////////////////////////////////////////////////////////////////////
    const Tags & JSONConfig::getPlatformTags() const
    {
        return m_platformTags;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::load( const InputStreamInterfacePtr & _stream, const DocumentPtr & _doc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, "invalid stream config" );

        LOGGER_INFO( "config", "load json config: %s"
            , Helper::getInputStreamDebugFilePath( _stream ).c_str()
        );

#if defined(MENGINE_DEBUG)
        m_debugFilePath = Helper::getInputStreamDebugFilePath( _stream );
#endif

        size_t size = _stream->size();

        MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryBuffer( _doc );

        Char * memory_buffer = memory->newBuffer( size + 1 );

        _stream->read( memory_buffer, size );
        memory_buffer[size] = '\0';

        if( memory_buffer[0] == 'R' && memory_buffer[1] == 'G' && memory_buffer[2] == 'C' && memory_buffer[3] == 'D' )
        {
            memory_buffer += 4;
            size -= 4;

            SECURE_SERVICE()
                ->unprotectData( 1, memory_buffer, size, memory_buffer );
        }

        jpp::object j = Helper::loadJSONStreamFromBuffer( memory_buffer, size, _doc );

        if( j.invalid() == true )
        {
            return false;
        }

        m_json = j;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONConfig::unload()
    {
        m_json = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::existValue( const Char * _section, const Char * _key ) const
    {
        ArrayString<128> platform_section;
        platform_section.append( _section );

        const VectorConstString & tags = m_platformTags.getValues();

        for( const ConstString & tag : tags )
        {
            platform_section.append( '-' );
            platform_section.append( tag );

            jpp::object j_tag_section;
            if( m_json.exist( platform_section.c_str(), &j_tag_section ) == false )
            {
                continue;
            }

            if( j_tag_section.exist( _key, nullptr ) == false )
            {
                continue;
            }

            return true;
        }

        jpp::object j_section;
        if( m_json.exist( _section, &j_section ) == false )
        {
            return false;
        }

        if( j_section.exist( _key, nullptr ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasValue( const Char * _section, const Char * _key, bool _default, bool * const _value ) const
    {
        return Detail::hasJSONValueT( m_json, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasValue( const Char * _section, const Char * _key, int8_t _default, int8_t * const  _value ) const
    {
        return Detail::hasJSONValueT( m_json, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasValue( const Char * _section, const Char * _key, uint8_t _default, uint8_t * const _value ) const
    {
        return Detail::hasJSONValueT( m_json, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasValue( const Char * _section, const Char * _key, int32_t _default, int32_t * const _value ) const
    {
        return Detail::hasJSONValueT( m_json, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasValue( const Char * _section, const Char * _key, uint32_t _default, uint32_t * const _value ) const
    {
        return Detail::hasJSONValueT( m_json, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasValue( const Char * _section, const Char * _key, int64_t _default, int64_t * const _value ) const
    {
        return Detail::hasJSONValueT( m_json, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasValue( const Char * _section, const Char * _key, uint64_t _default, uint64_t * const _value ) const
    {
        return Detail::hasJSONValueT( m_json, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasValue( const Char * _section, const Char * _key, float _default, float * const _value ) const
    {
        return Detail::hasJSONValueT( m_json, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasValue( const Char * _section, const Char * _key, double _default, double * const _value ) const
    {
        return Detail::hasJSONValueT( m_json, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasValue( const Char * _section, const Char * _key, const Char * _default, const Char ** const _value ) const
    {
        return Detail::hasJSONValueT( m_json, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasValue( const Char * _section, const Char * _key, const ConstString & _default, ConstString * const _value ) const
    {
        return Detail::hasJSONValueT( m_json, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasValue( const Char * _section, const Char * _key, const FilePath & _default, FilePath * const _value ) const
    {
        return Detail::hasJSONValueT( m_json, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasValue( const Char * _section, const Char * _key, const Tags & _default, Tags * const _value ) const
    {
        return Detail::hasJSONValueT( m_json, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasValue( const Char * _section, const Char * _key, const Resolution & _default, Resolution * const _value ) const
    {
        return Detail::hasJSONValueT( m_json, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasValue( const Char * _section, const Char * _key, const Color & _default, Color * const _value ) const
    {
        return Detail::hasJSONValueT( m_json, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::setValue( const Char * _section, const Char * _key, bool _value )
    {
        Detail::setJSONValueT( m_json, _section, _key, _value );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::setValue( const Char * _section, const Char * _key, int8_t _value )
    {
        Detail::setJSONValueT( m_json, _section, _key, _value );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::setValue( const Char * _section, const Char * _key, uint8_t _value )
    {
        Detail::setJSONValueT( m_json, _section, _key, _value );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::setValue( const Char * _section, const Char * _key, int32_t _value )
    {
        Detail::setJSONValueT( m_json, _section, _key, _value );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::setValue( const Char * _section, const Char * _key, uint32_t _value )
    {
        Detail::setJSONValueT( m_json, _section, _key, _value );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::setValue( const Char * _section, const Char * _key, int64_t _value )
    {
        Detail::setJSONValueT( m_json, _section, _key, _value );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::setValue( const Char * _section, const Char * _key, uint64_t _value )
    {
        Detail::setJSONValueT( m_json, _section, _key, _value );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::setValue( const Char * _section, const Char * _key, float _value )
    {
        Detail::setJSONValueT( m_json, _section, _key, _value );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::setValue( const Char * _section, const Char * _key, double _value )
    {
        Detail::setJSONValueT( m_json, _section, _key, _value );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::setValue( const Char * _section, const Char * _key, const Char * _value )
    {
        Detail::setJSONValueT( m_json, _section, _key, _value );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::setValue( const Char * _section, const Char * _key, const ConstString & _value )
    {
        Detail::setJSONValueT( m_json, _section, _key, _value );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::setValue( const Char * _section, const Char * _key, const FilePath & _value )
    {
        Detail::setJSONValueT( m_json, _section, _key, _value );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::setValue( const Char * _section, const Char * _key, const Tags & _value )
    {
        Detail::setJSONValueT( m_json, _section, _key, _value );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::setValue( const Char * _section, const Char * _key, const Resolution & _value )
    {
        Detail::setJSONValueT( m_json, _section, _key, _value );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::setValue( const Char * _section, const Char * _key, const Color & _value )
    {
        Detail::setJSONValueT( m_json, _section, _key, _value );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONConfig::getValues( const Char * _section, const Char * _key, VectorAspectRatioViewports * const _values ) const
    {
        Detail::calcJSONValuesT( m_json, m_platformTags, _section, _key, _values );
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONConfig::getValues( const Char * _section, const Char * _key, VectorFilePath * const _values ) const
    {
        Detail::calcJSONValuesT( m_json, m_platformTags, _section, _key, _values );
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONConfig::getValues( const Char * _section, const Char * _key, VectorConstString * const _values ) const
    {
        Detail::calcJSONValuesT( m_json, m_platformTags, _section, _key, _values );
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONConfig::getValues( const Char * _section, const Char * _key, VectorString * const _values ) const
    {
        Detail::calcJSONValuesT( m_json, m_platformTags, _section, _key, _values );
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONConfig::hasSection( const Char * _section ) const
    {
        ArrayString<128> platform_section;
        platform_section.append( _section );

        const VectorConstString & tags = m_platformTags.getValues();

        for( const ConstString & tag : tags )
        {
            platform_section.append( '-' );
            platform_section.append( tag );

            if( m_json.exist( platform_section.c_str(), nullptr ) == false )
            {
                continue;
            }

            return true;
        }

        if( m_json.exist( _section, nullptr ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
