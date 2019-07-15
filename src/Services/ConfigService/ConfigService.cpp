#include "ConfigService.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/PlatformInterface.h"

#include "Kernel/Exception.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ConfigService, Mengine::ConfigService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ConfigService::ConfigService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ConfigService::~ConfigService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ConfigService::_initializeService()
    {
        SERVICE_WAIT( PlatformInterface, [this]()
        {
            const Tags & platformTags = PLATFORM_SERVICE()
                ->getPlatformTags();

            m_platformTags = platformTags;

            return true;
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ConfigService::_finalizeService()
    {
        m_platformTags.clear();
        m_stores.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool ConfigService::loadConfig( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
    {
        InputStreamInterfacePtr stream = FILE_SERVICE()
            ->openInputFile( _fileGroup, _filePath, false, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, false, "invalid open public config '%s'"
            , _filePath.c_str()
        );

        IniUtil::IniStore store;
        if( IniUtil::loadIni( store, stream ) == false )
        {
            LOGGER_ERROR( "invalid load public config '%s'"
                , _filePath.c_str()
            );

            return false;
        }

        m_stores.emplace_back( store );

        return true;
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
    bool ConfigService::getValue( const Char * _section, const Char * _key, bool _default ) const
    {
        return Helper::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t ConfigService::getValue( const Char * _section, const Char * _key, int32_t _default ) const
    {
        return Helper::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ConfigService::getValue( const Char * _section, const Char * _key, uint32_t _default ) const
    {
        return Helper::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t ConfigService::getValue( const Char * _section, const Char * _key, uint64_t _default ) const
    {
        return Helper::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    float ConfigService::getValue( const Char * _section, const Char * _key, float _default ) const
    {
        return Helper::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    double ConfigService::getValue( const Char * _section, const Char * _key, double _default ) const
    {
        return Helper::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * ConfigService::getValue( const Char * _section, const Char * _key, const Char * _default ) const
    {
        return Helper::s_getValueString( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    ConstString ConfigService::getValue( const Char * _section, const Char * _key, const ConstString & _default ) const
    {
        return Helper::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    FilePath ConfigService::getValue( const Char * _section, const Char * _key, const FilePath & _default ) const
    {
        return Helper::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    Resolution ConfigService::getValue( const Char * _section, const Char * _key, const Resolution & _default ) const
    {
        return Helper::s_getValueT( m_stores, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    void ConfigService::getValues( const Char * _section, const Char * _key, VectorAspectRatioViewports & _values ) const
    {
        Helper::s_calcValueT( m_stores, m_platformTags, _section, _key, _values );
    }
    //////////////////////////////////////////////////////////////////////////
    void ConfigService::getValues( const Char * _section, const Char * _key, VectorConstString & _values ) const
    {
        Helper::s_calcValueT( m_stores, m_platformTags, _section, _key, _values );
    }
    //////////////////////////////////////////////////////////////////////////
    void ConfigService::getValues( const Char * _section, const Char * _key, VectorString & _values ) const
    {
        Helper::s_calcValueT( m_stores, m_platformTags, _section, _key, _values );
    }
    //////////////////////////////////////////////////////////////////////////
    void ConfigService::getSection( const Char * _section, MapParams & _params ) const
    {
        for( const IniUtil::IniStore & ini : m_stores )
        {
            IniUtil::getIniAllSettings( ini, _section, _params );
        }
    }
}
