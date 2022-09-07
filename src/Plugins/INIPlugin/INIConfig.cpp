#include "INIConfig.h"

#include "Kernel/Exception.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ArrayString.h"
#include "Kernel/FileStreamHelper.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////        
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static bool hasValueT2( const INIUtils::IniStore & _ini, const Char * _prefix, const Tags & _platform, const Char * _section, const Char * _key, T * const _value )
        {
            ArrayString<128> platform_section;
            platform_section.append( _prefix );
            platform_section.append( _section );

            const VectorConstString & tags = _platform.getValues();

            for( const ConstString & tag : tags )
            {
                platform_section.append( '-' );
                platform_section.append( tag );

                if( INIUtils::getIniValue( _ini, platform_section.c_str(), _key, _value ) == true )
                {
                    return true;
                }
            }

            ArrayString<128> section;
            section.append( _prefix );
            section.append( _section );

            if( INIUtils::getIniValue( _ini, section.c_str(), _key, _value ) == true )
            {
                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static bool hasValueT( const VectorIniStores & _stores, const Tags & _platform, const Char * _section, const Char * _key, T _default, T * const _value )
        {
            for( const INIUtils::IniStore & ini : _stores )
            {
#ifdef MENGINE_BUILD_PUBLISH
                if( Detail::hasValueT2( ini, "Publish-", _platform, _section, _key, _value ) == true )
                {
                    return true;
                }
#endif

                if( Detail::hasValueT2( ini, MENGINE_MASTER_RELEASE_VALUE( "Alpha-", "Develop-" ), _platform, _section, _key, _value ) == true )
                {
                    return true;
                }

                if( Detail::hasValueT2( ini, "", _platform, _section, _key, _value ) == true )
                {
                    return true;
                }
            }

            *_value = _default;

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static void calcValuesT2( const INIUtils::IniStore & _ini, const Char * _prefix, const Tags & _platform, const Char * _section, const Char * _key, T * const _value )
        {
            ArrayString<128> platform_section;
            platform_section.append( _prefix );
            platform_section.append( _section );

            const VectorConstString & tags = _platform.getValues();

            for( const ConstString & tag : tags )
            {
                platform_section.append( '-' );
                platform_section.append( tag );

                INIUtils::getIniValues( _ini, platform_section.c_str(), _key, _value );
            }

            ArrayString<128> section;
            section.append( _prefix );
            section.append( _section );

            INIUtils::getIniValues( _ini, section.c_str(), _key, _value );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static void calcValuesT( const VectorIniStores & _stores, const Tags & _platform, const Char * _section, const Char * _key, T * const _value )
        {
            for( const INIUtils::IniStore & ini : _stores )
            {
#ifdef MENGINE_BUILD_PUBLISH
                Detail::calcValuesT2( ini, "Publish-", _platform, _section, _key, _value );
#endif

                Detail::calcValuesT2( ini, MENGINE_MASTER_RELEASE_VALUE( "Alpha-", "Develop-" ), _platform, _section, _key, _value );

                Detail::calcValuesT2( ini, "", _platform, _section, _key, _value );
            }
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    INIConfig::INIConfig()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    INIConfig::~INIConfig()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void INIConfig::setPlatformTags( const Tags & _platformTags )
    {
        m_platformTags = _platformTags;
    }
    //////////////////////////////////////////////////////////////////////////
    const Tags & INIConfig::getPlatformTags() const
    {
        return m_platformTags;
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::load( const InputStreamInterfacePtr & _stream, const DocumentPtr & _doc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, "invalid stream config" );

        INIUtils::IniStore store;
        store.path = Helper::getInputStreamDebugFilePath( _stream );

        if( INIUtils::loadIni( store, _stream, _doc ) == false )
        {
            LOGGER_ERROR( "invalid load config" );

            return false;
        }

        m_stores.emplace_back( store );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void INIConfig::unload()
    {
        m_stores.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::existValue( const Char * _section, const Char * _key ) const
    {
        for( const INIUtils::IniStore & ini : m_stores )
        {
            ArrayString<128> platform_section;
            platform_section.append( _section );

            const VectorConstString & tags = m_platformTags.getValues();

            for( const ConstString & tag : tags )
            {
                platform_section.append( '-' );
                platform_section.append( tag );

                if( INIUtils::hasIniValue( ini, platform_section.c_str(), _key ) == true )
                {
                    return true;
                }
            }

            if( INIUtils::hasIniValue( ini, _section, _key ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::hasValue( const Char * _section, const Char * _key, bool _default, bool * const _value ) const
    {
        return Detail::hasValueT( m_stores, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::hasValue( const Char * _section, const Char * _key, int8_t _default, int8_t * const  _value ) const
    {
        return Detail::hasValueT( m_stores, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::hasValue( const Char * _section, const Char * _key, uint8_t _default, uint8_t * const _value ) const
    {
        return Detail::hasValueT( m_stores, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::hasValue( const Char * _section, const Char * _key, int32_t _default, int32_t * const _value ) const
    {
        return Detail::hasValueT( m_stores, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::hasValue( const Char * _section, const Char * _key, uint32_t _default, uint32_t * const _value ) const
    {
        return Detail::hasValueT( m_stores, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::hasValue( const Char * _section, const Char * _key, int64_t _default, int64_t * const _value ) const
    {
        return Detail::hasValueT( m_stores, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::hasValue( const Char * _section, const Char * _key, uint64_t _default, uint64_t * const _value ) const
    {
        return Detail::hasValueT( m_stores, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::hasValue( const Char * _section, const Char * _key, float _default, float * const _value ) const
    {
        return Detail::hasValueT( m_stores, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::hasValue( const Char * _section, const Char * _key, double _default, double * const _value ) const
    {
        return Detail::hasValueT( m_stores, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::hasValue( const Char * _section, const Char * _key, const Char * _default, const Char ** const _value ) const
    {
        return Detail::hasValueT( m_stores, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::hasValue( const Char * _section, const Char * _key, const ConstString & _default, ConstString * const _value ) const
    {
        return Detail::hasValueT( m_stores, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::hasValue( const Char * _section, const Char * _key, const FilePath & _default, FilePath * const _value ) const
    {
        return Detail::hasValueT( m_stores, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::hasValue( const Char * _section, const Char * _key, const Tags & _default, Tags * const _value ) const
    {
        return Detail::hasValueT( m_stores, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::hasValue( const Char * _section, const Char * _key, const Resolution & _default, Resolution * const _value ) const
    {
        return Detail::hasValueT( m_stores, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::hasValue( const Char * _section, const Char * _key, const Color & _default, Color * const _value ) const
    {
        return Detail::hasValueT( m_stores, m_platformTags, _section, _key, _default, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::setValue( const Char * _section, const Char * _key, bool _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::setValue( const Char * _section, const Char * _key, int8_t _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::setValue( const Char * _section, const Char * _key, uint8_t _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::setValue( const Char * _section, const Char * _key, int32_t _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::setValue( const Char * _section, const Char * _key, uint32_t _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::setValue( const Char * _section, const Char * _key, int64_t _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::setValue( const Char * _section, const Char * _key, uint64_t _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::setValue( const Char * _section, const Char * _key, float _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::setValue( const Char * _section, const Char * _key, double _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::setValue( const Char * _section, const Char * _key, const Char * _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::setValue( const Char * _section, const Char * _key, const ConstString & _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::setValue( const Char * _section, const Char * _key, const FilePath & _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::setValue( const Char * _section, const Char * _key, const Tags & _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::setValue( const Char * _section, const Char * _key, const Resolution & _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::setValue( const Char * _section, const Char * _key, const Color & _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void INIConfig::getValues( const Char * _section, const Char * _key, VectorAspectRatioViewports * const _values ) const
    {
        Detail::calcValuesT( m_stores, m_platformTags, _section, _key, _values );
    }
    //////////////////////////////////////////////////////////////////////////
    void INIConfig::getValues( const Char * _section, const Char * _key, VectorFilePath * const _values ) const
    {
        Detail::calcValuesT( m_stores, m_platformTags, _section, _key, _values );
    }
    //////////////////////////////////////////////////////////////////////////
    void INIConfig::getValues( const Char * _section, const Char * _key, VectorConstString * const _values ) const
    {
        Detail::calcValuesT( m_stores, m_platformTags, _section, _key, _values );
    }
    //////////////////////////////////////////////////////////////////////////
    void INIConfig::getValues( const Char * _section, const Char * _key, VectorString * const _values ) const
    {
        Detail::calcValuesT( m_stores, m_platformTags, _section, _key, _values );
    }
    //////////////////////////////////////////////////////////////////////////
    bool INIConfig::hasSection( const Char * _section ) const
    {
        for( const INIUtils::IniStore & ini : m_stores )
        {
            ArrayString<128> platform_section;
            platform_section.append( _section );

            const VectorConstString & tags = m_platformTags.getValues();

            for( const ConstString & tag : tags )
            {
                platform_section.append( '-' );
                platform_section.append( tag );

                if( INIUtils::hasIniSection( ini, platform_section.c_str() ) == true )
                {
                    return true;
                }
            }

            if( INIUtils::hasIniSection( ini, _section ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}
