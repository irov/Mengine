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
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, bool * const _value ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _value ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, int8_t * const _value ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _value ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, uint8_t * const _value ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _value ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, int32_t * const _value ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _value ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, uint32_t * const _value ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _value ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, int64_t * const  _value ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _value ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, uint64_t * const  _value ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _value ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, float * const _value ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _value ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, double * const _value ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _value ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, const Char ** const _value ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _value ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, ConstString * const _value ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _value ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, FilePath * const _value ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _value ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, Tags * const _value ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _value ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, Resolution * const _value ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _value ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::hasValue( const Char * _section, const Char * _key, Color * const _value ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            if( config->hasValue( _section, _key, _value ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MultiConfig::getValue( const Char * _section, const Char * _key, bool _default ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            bool value;
            if( config->hasValue( _section, _key, &value ) == false )
            {
                continue;
            }

            return value;
        }

        return _default;
    }
    //////////////////////////////////////////////////////////////////////////
    int8_t MultiConfig::getValue( const Char * _section, const Char * _key, int8_t _default ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            int8_t value;
            if( config->hasValue( _section, _key, &value ) == false )
            {
                continue;
            }

            return value;
        }

        return _default;
    }
    //////////////////////////////////////////////////////////////////////////
    uint8_t MultiConfig::getValue( const Char * _section, const Char * _key, uint8_t _default ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            uint8_t value;
            if( config->hasValue( _section, _key, &value ) == false )
            {
                continue;
            }

            return value;
        }

        return _default;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t MultiConfig::getValue( const Char * _section, const Char * _key, int32_t _default ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            int32_t value;
            if( config->hasValue( _section, _key, &value ) == false )
            {
                continue;
            }

            return value;
        }

        return _default;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MultiConfig::getValue( const Char * _section, const Char * _key, uint32_t _default ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            uint32_t value;
            if( config->hasValue( _section, _key, &value ) == false )
            {
                continue;
            }

            return value;
        }

        return _default;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t MultiConfig::getValue( const Char * _section, const Char * _key, int64_t _default ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            int64_t value;
            if( config->hasValue( _section, _key, &value ) == false )
            {
                continue;
            }

            return value;
        }

        return _default;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t MultiConfig::getValue( const Char * _section, const Char * _key, uint64_t _default ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            uint64_t value;
            if( config->hasValue( _section, _key, &value ) == false )
            {
                continue;
            }

            return value;
        }

        return _default;
    }
    //////////////////////////////////////////////////////////////////////////
    float MultiConfig::getValue( const Char * _section, const Char * _key, float _default ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            float value;
            if( config->hasValue( _section, _key, &value ) == false )
            {
                continue;
            }

            return value;
        }

        return _default;
    }
    //////////////////////////////////////////////////////////////////////////
    double MultiConfig::getValue( const Char * _section, const Char * _key, double _default ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            double value;
            if( config->hasValue( _section, _key, &value ) == false )
            {
                continue;
            }

            return value;
        }

        return _default;
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * MultiConfig::getValue( const Char * _section, const Char * _key, const Char * _default ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            const Char * value;
            if( config->hasValue( _section, _key, &value ) == false )
            {
                continue;
            }

            return value;
        }

        return _default;
    }
    //////////////////////////////////////////////////////////////////////////
    ConstString MultiConfig::getValue( const Char * _section, const Char * _key, const ConstString & _default ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            ConstString value;
            if( config->hasValue( _section, _key, &value ) == false )
            {
                continue;
            }

            return value;
        }

        return _default;
    }
    //////////////////////////////////////////////////////////////////////////
    FilePath MultiConfig::getValue( const Char * _section, const Char * _key, const FilePath & _default ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            FilePath value;
            if( config->hasValue( _section, _key, &value ) == false )
            {
                continue;
            }

            return value;
        }

        return _default;
    }
    //////////////////////////////////////////////////////////////////////////
    Tags MultiConfig::getValue( const Char * _section, const Char * _key, const Tags & _default ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            Tags value;
            if( config->hasValue( _section, _key, &value ) == false )
            {
                continue;
            }

            return value;
        }

        return _default;
    }
    //////////////////////////////////////////////////////////////////////////
    Resolution MultiConfig::getValue( const Char * _section, const Char * _key, const Resolution & _default ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            Resolution value;
            if( config->hasValue( _section, _key, &value ) == false )
            {
                continue;
            }

            return value;
        }

        return _default;
    }
    //////////////////////////////////////////////////////////////////////////
    Color MultiConfig::getValue( const Char * _section, const Char * _key, const Color & _default ) const
    {
        for( const ConfigInterfacePtr & config : m_configs )
        {
            Color value;
            if( config->hasValue( _section, _key, &value ) == false )
            {
                continue;
            }

            return value;
        }

        return _default;
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
    void MultiConfig::setValue( const Char * _section, const Char * _key, const Char * _value )
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _value );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();
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
