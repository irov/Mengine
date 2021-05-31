#include "IniConfig.h"

#include "Kernel/Exception.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ArrayString.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////        
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool s_hasValueString2( const IniUtil::IniStore & _ini, const Char * _prefix, const Tags & _platform, const Char * _section, const Char * _key, const Char ** const _value )
        {
            ArrayString<128> platform_section;
            platform_section.append( _prefix );
            platform_section.append( _section );

            const VectorConstString & tags = _platform.getValues();

            for( const ConstString & tag : tags )
            {
                platform_section.append( '-' );
                platform_section.append( tag );

                if( IniUtil::getIniValue( _ini, platform_section.c_str(), _key, _value ) == true )
                {
                    return true;
                }
            }

            ArrayString<128> section;
            section.append( _prefix );
            section.append( _section );

            if( IniUtil::getIniValue( _ini, section.c_str(), _key, _value ) == true )
            {
                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_hasValueString( const VectorIniStores & _stores, const Tags & _platform, const Char * _section, const Char * _key, const Char ** const _value )
        {
            for( const IniUtil::IniStore & ini : _stores )
            {
#ifdef MENGINE_BUILD_PUBLISH
                if( Detail::s_hasValueString2( ini, "Publish-", _platform, _section, _key, _value ) == true )
                {
                    return true;
                }
#endif

                if( Detail::s_hasValueString2( ini, MENGINE_MASTER_VALUE( "Alpha-", "Develop-" ), _platform, _section, _key, _value ) == true )
                {
                    return true;
                }

#ifdef MENGINE_MASTER_RELEASE
                if( Detail::s_hasValueString2( ini, "Master-", _platform, _section, _key, _value ) == true )
                {
                    return true;
                }
#endif

                if( Detail::s_hasValueString2( ini, "", _platform, _section, _key, _value ) == true )
                {
                    return true;
                }
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static bool s_hasValueT2( const IniUtil::IniStore & _ini, const Char * _prefix, const Tags & _platform, const Char * _section, const Char * _key, T * _value )
        {
            ArrayString<128> platform_section;
            platform_section.append( _prefix );
            platform_section.append( _section );

            const VectorConstString & tags = _platform.getValues();

            for( const ConstString & tag : tags )
            {
                platform_section.append( '-' );
                platform_section.append( tag );

                if( IniUtil::getIniValue( _ini, platform_section.c_str(), _key, _value ) == true )
                {
                    return true;
                }
            }

            ArrayString<128> section;
            section.append( _prefix );
            section.append( _section );

            if( IniUtil::getIniValue( _ini, section.c_str(), _key, _value ) == true )
            {
                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static bool s_hasValueT( const VectorIniStores & _stores, const Tags & _platform, const Char * _section, const Char * _key, T * _value )
        {
            for( const IniUtil::IniStore & ini : _stores )
            {
#ifdef MENGINE_BUILD_PUBLISH
                if( Detail::s_hasValueT2( ini, "Publish-", _platform, _section, _key, _value ) == true )
                {
                    return true;
                }
#endif

                if( Detail::s_hasValueT2( ini, MENGINE_MASTER_VALUE( "Alpha-", "Develop-" ), _platform, _section, _key, _value ) == true )
                {
                    return true;
                }

#ifdef MENGINE_MASTER_RELEASE
                if( Detail::s_hasValueT2( ini, "Master-", _platform, _section, _key, _value ) == true )
                {
                    return true;
                }
#endif

                if( Detail::s_hasValueT2( ini, "", _platform, _section, _key, _value ) == true )
                {
                    return true;
                }
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_getValueString2( const IniUtil::IniStore & _ini, const Char * _prefix, const Tags & _platform, const Char * _section, const Char * _key, const Char ** const _value )
        {
            ArrayString<128> platform_section;
            platform_section.append( _prefix );
            platform_section.append( _section );

            const VectorConstString & tags = _platform.getValues();

            for( const ConstString & tag : tags )
            {
                platform_section.append( '-' );
                platform_section.append( tag );

                if( IniUtil::getIniValue( _ini, platform_section.c_str(), _key, _value ) == true )
                {
                    return true;
                }
            }

            ArrayString<128> section;
            section.append( _prefix );
            section.append( _section );

            if( IniUtil::getIniValue( _ini, section.c_str(), _key, _value ) == true )
            {
                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static const Char * s_getValueString( const VectorIniStores & _stores, const Tags & _platform, const Char * _section, const Char * _key, const Char * _default )
        {
            for( const IniUtil::IniStore & ini : _stores )
            {
                const Char * value;

#ifdef MENGINE_BUILD_PUBLISH
                if( Detail::s_getValueString2( ini, "Publish-", _platform, _section, _key, &value ) == true )
                {
                    return value;
                }
#endif

                if( Detail::s_getValueString2( ini, MENGINE_MASTER_VALUE( "Alpha-", "Develop-" ), _platform, _section, _key, &value ) == true )
                {
                    return value;
                }

#ifdef MENGINE_MASTER_RELEASE
                if( Detail::s_getValueString2( ini, "Master-", _platform, _section, _key, &value ) == true )
                {
                    return value;
                }
#endif

                if( Detail::s_getValueString2( ini, "", _platform, _section, _key, &value ) == true )
                {
                    return value;
                }
            }

            return _default;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static bool s_getValueT2( const IniUtil::IniStore & _ini, const Char * _prefix, const Tags & _platform, const Char * _section, const Char * _key, T * _value )
        {
            ArrayString<128> platform_section;
            platform_section.append( _prefix );
            platform_section.append( _section );

            const VectorConstString & tags = _platform.getValues();

            for( const ConstString & tag : tags )
            {
                platform_section.append( '-' );
                platform_section.append( tag );

                if( IniUtil::getIniValue( _ini, platform_section.c_str(), _key, _value ) == true )
                {
                    return true;
                }
            }

            ArrayString<128> section;
            section.append( _prefix );
            section.append( _section );

            if( IniUtil::getIniValue( _ini, section.c_str(), _key, _value ) == true )
            {
                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static T s_getValueT( const VectorIniStores & _stores, const Tags & _platform, const Char * _section, const Char * _key, T _default )
        {
            for( const IniUtil::IniStore & ini : _stores )
            {
                T value;

#ifdef MENGINE_BUILD_PUBLISH
                if( Detail::s_getValueT2( ini, "Publish-", _platform, _section, _key, &value ) == true )
                {
                    return value;
                }
#endif

                if( Detail::s_getValueT2( ini, MENGINE_MASTER_VALUE( "Alpha-", "Develop-" ), _platform, _section, _key, &value ) == true )
                {
                    return value;
                }

#ifdef MENGINE_MASTER_RELEASE
                if( Detail::s_getValueT2( ini, "Master-", _platform, _section, _key, &value ) == true )
                {
                    return value;
                }
#endif

                if( Detail::s_getValueT2( ini, "", _platform, _section, _key, &value ) == true )
                {
                    return value;
                }
            }

            return _default;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static void s_calcValuesT2( const IniUtil::IniStore & _ini, const Char * _prefix, const Tags & _platform, const Char * _section, const Char * _key, T * _value )
        {
            ArrayString<128> platform_section;
            platform_section.append( _prefix );
            platform_section.append( _section );

            const VectorConstString & tags = _platform.getValues();

            for( const ConstString & tag : tags )
            {
                platform_section.append( '-' );
                platform_section.append( tag );

                if( IniUtil::hasIniValue( _ini, platform_section.c_str(), _key ) == false )
                {
                    continue;
                }

                if( IniUtil::getIniValues( _ini, platform_section.c_str(), _key, _value ) == true )
                {
                    break;
                }
            }

            ArrayString<128> section;
            section.append( _prefix );
            section.append( _section );

            IniUtil::getIniValues( _ini, section.c_str(), _key, _value );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static void s_calcValuesT( const VectorIniStores & _stores, const Tags & _platform, const Char * _section, const Char * _key, T * _value )
        {
            for( const IniUtil::IniStore & ini : _stores )
            {
#ifdef MENGINE_BUILD_PUBLISH
                Detail::s_calcValuesT2( ini, "Publish-", _platform, _section, _key, _value );
#endif

                Detail::s_calcValuesT2( ini, MENGINE_MASTER_VALUE( "Alpha-", "Develop-" ), _platform, _section, _key, _value );

#ifdef MENGINE_MASTER_RELEASE
                Detail::s_calcValuesT2( ini, "Master-", _platform, _section, _key, _value );
#endif

                Detail::s_calcValuesT2( ini, "", _platform, _section, _key, _value );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    IniConfig::IniConfig()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    IniConfig::~IniConfig()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void IniConfig::setPlatformTags( const Tags & _platformTags )
    {
        m_platformTags = _platformTags;
    }
    //////////////////////////////////////////////////////////////////////////
    const Tags & IniConfig::getPlatformTags() const
    {
        return m_platformTags;
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::load( const InputStreamInterfacePtr & _stream, const DocumentPtr & _doc )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, "invalid stream config" );

        IniUtil::IniStore store;
        if( IniUtil::loadIni( store, _stream, _doc ) == false )
        {
            LOGGER_ERROR( "invalid load config" );

            return false;
        }

        m_stores.emplace_back( store );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::existValue( const Char * _section, const Char * _key ) const
    {
        for( const IniUtil::IniStore & ini : m_stores )
        {
            ArrayString<128> platform_section;
            platform_section.append( _section );

            const VectorConstString & tags = m_platformTags.getValues();

            for( const ConstString & tag : tags )
            {
                platform_section.append( '-' );
                platform_section.append( tag );

                if( IniUtil::hasIniValue( ini, platform_section.c_str(), _key ) == true )
                {
                    return true;
                }
            }

            if( IniUtil::hasIniValue( ini, _section, _key ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::hasValue( const Char * _section, const Char * _key, bool * const _value ) const
    {
        return Detail::s_hasValueT( m_stores, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::hasValue( const Char * _section, const Char * _key, int8_t * const  _value ) const
    {
        return Detail::s_hasValueT( m_stores, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::hasValue( const Char * _section, const Char * _key, uint8_t * const _value ) const
    {
        return Detail::s_hasValueT( m_stores, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::hasValue( const Char * _section, const Char * _key, int32_t * const _value ) const
    {
        return Detail::s_hasValueT( m_stores, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::hasValue( const Char * _section, const Char * _key, uint32_t * const _value ) const
    {
        return Detail::s_hasValueT( m_stores, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::hasValue( const Char * _section, const Char * _key, uint64_t * const _value ) const
    {
        return Detail::s_hasValueT( m_stores, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::hasValue( const Char * _section, const Char * _key, float * const _value ) const
    {
        return Detail::s_hasValueT( m_stores, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::hasValue( const Char * _section, const Char * _key, double * const _value ) const
    {
        return Detail::s_hasValueT( m_stores, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::hasValue( const Char * _section, const Char * _key, const Char ** const _value ) const
    {
        return Detail::s_hasValueString( m_stores, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::hasValue( const Char * _section, const Char * _key, ConstString * const _value ) const
    {
        return Detail::s_hasValueT( m_stores, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::hasValue( const Char * _section, const Char * _key, FilePath * const _value ) const
    {
        return Detail::s_hasValueT( m_stores, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::hasValue( const Char * _section, const Char * _key, Tags * const _value ) const
    {
        return Detail::s_hasValueT( m_stores, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::hasValue( const Char * _section, const Char * _key, Resolution * const _value ) const
    {
        return Detail::s_hasValueT( m_stores, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::hasValue( const Char * _section, const Char * _key, Color * const _value ) const
    {
        return Detail::s_hasValueT( m_stores, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::getValue( const Char * _section, const Char * _key, bool _default ) const
    {
        return Detail::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    int8_t IniConfig::getValue( const Char * _section, const Char * _key, int8_t _default ) const
    {
        return Detail::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    uint8_t IniConfig::getValue( const Char * _section, const Char * _key, uint8_t _default ) const
    {
        return Detail::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t IniConfig::getValue( const Char * _section, const Char * _key, int32_t _default ) const
    {
        return Detail::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t IniConfig::getValue( const Char * _section, const Char * _key, uint32_t _default ) const
    {
        return Detail::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t IniConfig::getValue( const Char * _section, const Char * _key, uint64_t _default ) const
    {
        return Detail::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    float IniConfig::getValue( const Char * _section, const Char * _key, float _default ) const
    {
        return Detail::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    double IniConfig::getValue( const Char * _section, const Char * _key, double _default ) const
    {
        return Detail::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * IniConfig::getValue( const Char * _section, const Char * _key, const Char * _default ) const
    {
        return Detail::s_getValueString( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    ConstString IniConfig::getValue( const Char * _section, const Char * _key, const ConstString & _default ) const
    {
        return Detail::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    FilePath IniConfig::getValue( const Char * _section, const Char * _key, const FilePath & _default ) const
    {
        return Detail::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    Tags IniConfig::getValue( const Char * _section, const Char * _key, const Tags & _default ) const
    {
        return Detail::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    Resolution IniConfig::getValue( const Char * _section, const Char * _key, const Resolution & _default ) const
    {
        return Detail::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    Color IniConfig::getValue( const Char * _section, const Char * _key, const Color & _default ) const
    {
        return Detail::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    void IniConfig::getValues( const Char * _section, const Char * _key, VectorAspectRatioViewports * const _values ) const
    {
        Detail::s_calcValuesT( m_stores, m_platformTags, _section, _key, _values );
    }
    //////////////////////////////////////////////////////////////////////////
    void IniConfig::getValues( const Char * _section, const Char * _key, VectorFilePath * const _values ) const
    {
        Detail::s_calcValuesT( m_stores, m_platformTags, _section, _key, _values );
    }
    //////////////////////////////////////////////////////////////////////////
    void IniConfig::getValues( const Char * _section, const Char * _key, VectorConstString * const _values ) const
    {
        Detail::s_calcValuesT( m_stores, m_platformTags, _section, _key, _values );
    }
    //////////////////////////////////////////////////////////////////////////
    void IniConfig::getValues( const Char * _section, const Char * _key, VectorString * const _values ) const
    {
        Detail::s_calcValuesT( m_stores, m_platformTags, _section, _key, _values );
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::hasSection( const Char * _section ) const
    {
        for( const IniUtil::IniStore & ini : m_stores )
        {
            if( IniUtil::hasIniSection( ini, _section ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void IniConfig::setValue( const Char * _section, const Char * _key, const Char * _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();
    }
    //////////////////////////////////////////////////////////////////////////
}
