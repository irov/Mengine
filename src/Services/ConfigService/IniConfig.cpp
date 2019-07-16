#include "INIConfig.h"

#include "Kernel/Exception.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
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
    bool IniConfig::load( const InputStreamInterfacePtr & _stream )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, false, "invalid stream config" );

        IniUtil::IniStore store;
        if( IniUtil::loadIni( store, _stream ) == false )
        {
            LOGGER_ERROR( "invalid load config" );

            return false;
        }

        m_stores.emplace_back( store );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////        
    namespace Helper
    {    
        //////////////////////////////////////////////////////////////////////////
        static bool s_hasValueString( const VectorIniStores & _stores, const Tags & _platform, const Char * _section, const Char * _key, const Char ** _value )
        {
            for( const IniUtil::IniStore & ini : _stores )
            {
                stdex::array_string<128> platform_section;
                platform_section.append( _section );

                const VectorConstString & tags = _platform.getValues();

                for( const ConstString & tag : tags )
                {
                    platform_section.append( '-' );
                    platform_section.append( tag );

                    const Char * value;
                    if( IniUtil::getIniValue( ini, platform_section.c_str(), _key, &value ) == true )
                    {
                        *_value = value;

                        return true;
                    }
                }

                const Char * value;
                if( IniUtil::getIniValue( ini, _section, _key, &value ) == true )
                {
                    *_value = value;

                    return true;
                }
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static bool s_hasValueT( const VectorIniStores & _stores, const Tags & _platform, const Char * _section, const Char * _key, T * _value )
        {
            for( const IniUtil::IniStore & ini : _stores )
            {
                stdex::array_string<128> platform_section;
                platform_section.append( _section );

                const VectorConstString & tags = _platform.getValues();

                for( const ConstString & tag : tags )
                {
                    platform_section.append( '-' );
                    platform_section.append( tag );

                    T value;
                    if( IniUtil::getIniValue( ini, platform_section.c_str(), _key, value ) == true )
                    {
                        *_value = value;

                        return true;
                    }
                }

                T value;
                if( IniUtil::getIniValue( ini, _section, _key, value ) == true )
                {
                    *_value = value;

                    return true;
                }
            }

            return false;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::hasValue( const Char * _section, const Char * _key, bool * _value ) const
    {
        return Helper::s_hasValueT( m_stores, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::hasValue( const Char * _section, const Char * _key, int32_t * _value ) const
    {
        return Helper::s_hasValueT( m_stores, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::hasValue( const Char * _section, const Char * _key, uint32_t * _value ) const
    {
        return Helper::s_hasValueT( m_stores, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::hasValue( const Char * _section, const Char * _key, uint64_t * _value ) const
    {
        return Helper::s_hasValueT( m_stores, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::hasValue( const Char * _section, const Char * _key, float * _value ) const
    {
        return Helper::s_hasValueT( m_stores, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::hasValue( const Char * _section, const Char * _key, double * _value ) const
    {
        return Helper::s_hasValueT( m_stores, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::hasValue( const Char * _section, const Char * _key, const Char ** _value ) const
    {
        return Helper::s_hasValueString( m_stores, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::hasValue( const Char * _section, const Char * _key, ConstString * _value ) const
    {
        return Helper::s_hasValueT( m_stores, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::hasValue( const Char * _section, const Char * _key, FilePath * _value ) const
    {
        return Helper::s_hasValueT( m_stores, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::hasValue( const Char * _section, const Char * _key, Tags * _value ) const
    {
        return Helper::s_hasValueT( m_stores, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::hasValue( const Char * _section, const Char * _key, Resolution * _value ) const
    {
        return Helper::s_hasValueT( m_stores, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::hasValue( const Char * _section, const Char * _key, Color * _value ) const
    {
        return Helper::s_hasValueT( m_stores, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        static const Char * s_getValueString( const VectorIniStores & _stores, const Tags & _platform, const Char * _section, const Char * _key, const Char * _default )
        {
            for( const IniUtil::IniStore & ini : _stores )
            {
                stdex::array_string<128> platform_section;
                platform_section.append( _section );

                const VectorConstString & tags = _platform.getValues();

                for( const ConstString & tag : tags )
                {
                    platform_section.append( '-' );
                    platform_section.append( tag );

                    const Char * value;
                    if( IniUtil::getIniValue( ini, platform_section.c_str(), _key, &value ) == true )
                    {
                        return value;
                    }
                }

                const Char * value;
                if( IniUtil::getIniValue( ini, _section, _key, &value ) == true )
                {
                    return value;
                }
            }

            return _default;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static T s_getValueT( const VectorIniStores & _stores, const Tags & _platform, const Char * _section, const Char * _key, T _default )
        {
            for( const IniUtil::IniStore & ini : _stores )
            {
                stdex::array_string<128> platform_section;
                platform_section.append( _section );

                const VectorConstString & tags = _platform.getValues();

                for( const ConstString & tag : tags )
                {
                    platform_section.append( '-' );
                    platform_section.append( tag );

                    T value;
                    if( IniUtil::getIniValue( ini, platform_section.c_str(), _key, value ) == true )
                    {
                        return value;
                    }
                }

                T value;
                if( IniUtil::getIniValue( ini, _section, _key, value ) == true )
                {
                    return value;
                }
            }

            return _default;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static void s_calcValueT( const VectorIniStores & _stores, const Tags & _platform, const Char * _section, const Char * _key, T & _value )
        {
            for( const IniUtil::IniStore & ini : _stores )
            {
                stdex::array_string<128> platform_section;
                platform_section.append( _section );

                const VectorConstString & tags = _platform.getValues();

                stdex::array_string<128> platform_section_found;

                for( const ConstString & tag : tags )
                {
                    platform_section.append( '-' );
                    platform_section.append( tag );

                    if( IniUtil::hasIniValue( ini, platform_section.c_str(), _key ) == false )
                    {
                        continue;
                    }

                    platform_section_found = platform_section;

                    break;
                }

                if( platform_section_found.empty() == false )
                {
                    if( IniUtil::getIniValue( ini, platform_section_found.c_str(), _key, _value ) == true )
                    {
                        return;
                    }
                }

                if( IniUtil::getIniValue( ini, _section, _key, _value ) == true )
                {
                    return;
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool IniConfig::getValue( const Char * _section, const Char * _key, bool _default ) const
    {
        return Helper::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t IniConfig::getValue( const Char * _section, const Char * _key, int32_t _default ) const
    {
        return Helper::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t IniConfig::getValue( const Char * _section, const Char * _key, uint32_t _default ) const
    {
        return Helper::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t IniConfig::getValue( const Char * _section, const Char * _key, uint64_t _default ) const
    {
        return Helper::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    float IniConfig::getValue( const Char * _section, const Char * _key, float _default ) const
    {
        return Helper::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    double IniConfig::getValue( const Char * _section, const Char * _key, double _default ) const
    {
        return Helper::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * IniConfig::getValue( const Char * _section, const Char * _key, const Char * _default ) const
    {
        return Helper::s_getValueString( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    ConstString IniConfig::getValue( const Char * _section, const Char * _key, const ConstString & _default ) const
    {
        return Helper::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    FilePath IniConfig::getValue( const Char * _section, const Char * _key, const FilePath & _default ) const
    {
        return Helper::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    Tags IniConfig::getValue( const Char * _section, const Char * _key, const Tags & _default ) const
    {
        return Helper::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    Resolution IniConfig::getValue( const Char * _section, const Char * _key, const Resolution & _default ) const
    {
        return Helper::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    Color IniConfig::getValue( const Char * _section, const Char * _key, const Color & _default ) const
    {
        return Helper::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    void IniConfig::getValues( const Char * _section, const Char * _key, VectorAspectRatioViewports & _values ) const
    {
        Helper::s_calcValueT( m_stores, m_platformTags, _section, _key, _values );
    }
    //////////////////////////////////////////////////////////////////////////
    void IniConfig::getValues( const Char * _section, const Char * _key, VectorFilePath & _values ) const
    {
        Helper::s_calcValueT( m_stores, m_platformTags, _section, _key, _values );
    }
    //////////////////////////////////////////////////////////////////////////
    void IniConfig::getValues( const Char * _section, const Char * _key, VectorConstString & _values ) const
    {
        Helper::s_calcValueT( m_stores, m_platformTags, _section, _key, _values );
    }
    //////////////////////////////////////////////////////////////////////////
    void IniConfig::getValues( const Char * _section, const Char * _key, VectorString & _values ) const
    {
        Helper::s_calcValueT( m_stores, m_platformTags, _section, _key, _values );
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
    void IniConfig::getSection( const Char * _section, MapParams & _params ) const
    {
        for( const IniUtil::IniStore & ini : m_stores )
        {
            IniUtil::getIniAllSettings( ini, _section, _params );
        }
    }
}
