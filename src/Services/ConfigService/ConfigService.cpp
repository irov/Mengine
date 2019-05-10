#include "ConfigService.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/PlatformInterface.h"

#include "Kernel/Exception.h"

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
        if( SERVICE_EXIST( Mengine::PlatformInterface ) == true )
        {
            const Tags & platformTags = PLATFORM_SERVICE()
                ->getPlatformTags();

            m_platformTags = platformTags;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ConfigService::_finalizeService()
    {
        m_platformTags.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool ConfigService::loadConfig( const FileGroupInterfacePtr & _fileGroup, const FilePath & _publicConfigPath, const FilePath & _privateConfigPath )
    {
        if( _fileGroup->existFile( _publicConfigPath ) == true )
        {
            InputStreamInterfacePtr stream = FILE_SERVICE()
                ->openInputFile( _fileGroup, _publicConfigPath, false, MENGINE_DOCUMENT_FUNCTION );

            if( stream == nullptr )
            {
                LOGGER_ERROR( "invalid open public config '%s'"
                    , _publicConfigPath.c_str()
                );

                return false;
            }

            if( IniUtil::loadIni( m_publicINI, stream ) == false )
            {
                LOGGER_ERROR( "invalid load public config '%s'"
                    , _publicConfigPath.c_str()
                );

                return false;
            }
        }

        if( _fileGroup->existFile( _privateConfigPath ) == true )
        {
            InputStreamInterfacePtr stream = FILE_SERVICE()
                ->openInputFile( _fileGroup, _privateConfigPath, false, MENGINE_DOCUMENT_FUNCTION );

            if( stream == nullptr )
            {
                LOGGER_ERROR( "invalid open private config '%s'"
                    , _privateConfigPath.c_str()
                );

                return false;
            }

            if( IniUtil::loadIni( m_publicINI, stream ) == false )
            {
                LOGGER_ERROR( "invalid load private config '%s'"
                    , _privateConfigPath.c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        static const Char * s_getValueString( const IniUtil::IniStore & _ini, const Tags & _platform, const Char * _section, const Char * _key, const Char * _default )
        {
            stdex::array_string<128> platform_section;
            platform_section.append( _section );

            const VectorConstString & tags = _platform.getValues();

            for( const ConstString & tag : tags )
            {
                platform_section.append( '-' );
                platform_section.append( tag );

                const Char * value;
                if( IniUtil::getIniValue( _ini, platform_section.c_str(), _key, &value ) == true )
                {
                    return value;
                }
            }

            const Char * value;
            if( IniUtil::getIniValue( _ini, _section, _key, &value ) == true )
            {
                return value;
            }

            return _default;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static T s_getValueT( const IniUtil::IniStore & _ini, const Tags & _platform, const Char * _section, const Char * _key, T _default )
        {
            stdex::array_string<128> platform_section;
            platform_section.append( _section );

            const VectorConstString & tags = _platform.getValues();

            for( const ConstString & tag : tags )
            {
                platform_section.append( '-' );
                platform_section.append( tag );

                T value;
                if( IniUtil::getIniValue( _ini, platform_section.c_str(), _key, value ) == true )
                {
                    return value;
                }
            }

            T value;
            if( IniUtil::getIniValue( _ini, _section, _key, value ) == true )
            {
                return value;
            }

            return _default;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static void s_calcValueT( const IniUtil::IniStore & _ini, const Tags & _platform, const Char * _section, const Char * _key, T & _value )
        {
            stdex::array_string<128> platform_section;
            platform_section.append( _section );

            const VectorConstString & tags = _platform.getValues();

            stdex::array_string<128> platform_section_found;

            for( const ConstString & tag : tags )
            {
                platform_section.append( '-' );
                platform_section.append( tag );

                if( IniUtil::hasIniValue( _ini, platform_section.c_str(), _key ) == false )
                {
                    continue;
                }

                platform_section_found = platform_section;

                break;
            }

            if( platform_section_found.empty() == false )
            {
                IniUtil::getIniValue( _ini, platform_section_found.c_str(), _key, _value );

                return;
            }

            IniUtil::getIniValue( _ini, _section, _key, _value );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool ConfigService::getValue( const Char * _section, const Char * _key, bool _default ) const
    {
        return Helper::s_getValueT( m_publicINI, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t ConfigService::getValue( const Char * _section, const Char * _key, int32_t _default ) const
    {
        return Helper::s_getValueT( m_publicINI, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ConfigService::getValue( const Char * _section, const Char * _key, uint32_t _default ) const
    {
        return Helper::s_getValueT( m_publicINI, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t ConfigService::getValue( const Char * _section, const Char * _key, uint64_t _default ) const
    {
        return Helper::s_getValueT( m_publicINI, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    float ConfigService::getValue( const Char * _section, const Char * _key, float _default ) const
    {
        return Helper::s_getValueT( m_publicINI, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    double ConfigService::getValue( const Char * _section, const Char * _key, double _default ) const
    {
        return Helper::s_getValueT( m_publicINI, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * ConfigService::getValue( const Char * _section, const Char * _key, const Char * _default ) const
    {
        return Helper::s_getValueString( m_publicINI, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    ConstString ConfigService::getValue( const Char * _section, const Char * _key, const ConstString & _default ) const
    {
        return Helper::s_getValueT( m_publicINI, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    FilePath ConfigService::getValue( const Char * _section, const Char * _key, const FilePath & _default ) const
    {
        return Helper::s_getValueT( m_publicINI, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    Resolution ConfigService::getValue( const Char * _section, const Char * _key, const Resolution & _default ) const
    {
        return Helper::s_getValueT( m_publicINI, m_platformTags, _section, _key, _default );
    }
    //////////////////////////////////////////////////////////////////////////
    void ConfigService::getValues( const Char * _section, const Char * _key, VectorAspectRatioViewports & _value ) const
    {
        Helper::s_calcValueT( m_publicINI, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    void ConfigService::getValues( const Char * _section, const Char * _key, VectorString & _value ) const
    {
        Helper::s_calcValueT( m_publicINI, m_platformTags, _section, _key, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    void ConfigService::getSection( const Char * _section, MapParams & _params ) const
    {
        IniUtil::getIniAllSettings( m_publicINI, _section, _params );
    }
}
