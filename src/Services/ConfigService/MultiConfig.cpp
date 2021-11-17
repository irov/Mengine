#include "MultiConfig.h"

#include "Kernel/Exception.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Stringalized.h"
#include "Kernel/ArrayString.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{   
    //////////////////////////////////////////////////////////////////////////
    MultiConfig::MultiConfig()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MultiConfig::~MultiConfig()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MultiConfig::addConfig( const ConfigInterfacePtr & _config )
    {
        m_configs.push_back( _config );
    }
    //////////////////////////////////////////////////////////////////////////
    void MultiConfig::foreachConfig( const LambdaConfigs & _lambda )
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            _lambda( config );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MultiConfig::setPlatformTags( const Tags & _platformTags )
    {
        m_platformTags = _platformTags;
    }
    //////////////////////////////////////////////////////////////////////////
    const Tags & MultiConfig::getPlatformTags() const
    {
        return m_platformTags;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::existValue( const Char * _section, const Char * _key ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->existValue( _section, _key ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::load( const InputStreamInterfacePtr & _stream, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _stream );
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MultiConfig::unload()
    {
        m_configs.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, bool _default, bool * const _value ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _default, _value ) == true )
            {
                return true;
            }
        }

        *_value = _default;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, int8_t _default, int8_t * const _value ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _default, _value ) == true )
            {
                return true;
            }
        }

        *_value = _default;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, uint8_t _default, uint8_t * const _value ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _default, _value ) == true )
            {
                return true;
            }
        }

        *_value = _default;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, int32_t _default, int32_t * const _value ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _default, _value ) == true )
            {
                return true;
            }
        }

        *_value = _default;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, uint32_t _default, uint32_t * const _value ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _default, _value ) == true )
            {
                return true;
            }
        }

        *_value = _default;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, int64_t _default, int64_t * const _value ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _default, _value ) == true )
            {
                return true;
            }
        }

        *_value = _default;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, uint64_t _default, uint64_t * const _value ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _default, _value ) == true )
            {
                return true;
            }
        }

        *_value = _default;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, float _default, float * const _value ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _default, _value ) == true )
            {
                return true;
            }
        }

        *_value = _default;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, double _default, double * const _value ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _default, _value ) == true )
            {
                return true;
            }
        }

        *_value = _default;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, const Char * _default, const Char ** const _value ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _default, _value ) == true )
            {
                return true;
            }
        }

        *_value = _default;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, const ConstString & _default, ConstString * const _value ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _default, _value ) == true )
            {
                return true;
            }
        }

        *_value = _default;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, const FilePath & _default, FilePath * const _value ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _default, _value ) == true )
            {
                return true;
            }
        }

        *_value = _default;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, const Tags & _default, Tags * const _value ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _default, _value ) == true )
            {
                return true;
            }
        }

        *_value = _default;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, const Resolution & _default, Resolution * const _value ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _default, _value ) == true )
            {
                return true;
            }
        }

        *_value = _default;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, const Color & _default, Color * const _value ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _default, _value ) == true )
            {
                return true;
            }
        }

        *_value = _default;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void MultiConfig::getValues( const Char * _section, const Char * _key, VectorAspectRatioViewports * const _values ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            config->getValues( _section, _key, _values );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MultiConfig::getValues( const Char * _section, const Char * _key, VectorFilePath * const _values ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            config->getValues( _section, _key, _values );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MultiConfig::getValues( const Char * _section, const Char * _key, VectorConstString * const _values ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            config->getValues( _section, _key, _values );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MultiConfig::getValues( const Char * _section, const Char * _key, VectorString * const _values ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            config->getValues( _section, _key, _values );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasSection( const Char * _section ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasSection( _section ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}
