#include "OptionsService.h"

#include "Kernel/Logger.h"

#include <string.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( OptionsService, Mengine::OptionsService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OptionsService::OptionsService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OptionsService::~OptionsService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool OptionsService::_initializeService()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OptionsService::_finalizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool OptionsService::setArguments( const ArgumentsInterfacePtr & _arguments )
    {
        uint32_t argument_count = _arguments->getArgumentCount();

        for( uint32_t index = 0; index != argument_count; ++index )
        {
            const Char * option_str = _arguments->getArgument( index );

            const Char * option_key_str = strchr( option_str, '-' );

            if( option_key_str == nullptr )
            {
                continue;
            }

            option_key_str += 1;

            const Char * option_value_str = strchr( option_key_str, ':' );

            Option op;

            if( option_value_str == nullptr )
            {
                if( strlen( option_key_str ) >= MENGINE_OPTIONS_KEY_SIZE )
                {
                    return false;
                }

                strcpy( op.key, option_key_str );

                op.value[0] = '\0';
            }
            else
            {
                size_t key_size = option_value_str - option_key_str;

                if( key_size >= MENGINE_OPTIONS_KEY_SIZE )
                {
                    return false;
                }

                strncpy( op.key, option_key_str, key_size );
                op.key[key_size] = '\0';

                if( strlen( option_value_str + 1 ) >= MENGINE_OPTIONS_VALUE_SIZE )
                {
                    return false;
                }

                strcpy( op.value, option_value_str + 1 );
            }

            m_options.push_back( op );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OptionsService::hasOption( const Char * _key ) const
    {
        for( const Option & op : m_options )
        {
            if( strcmp( op.key, _key ) != 0 )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * OptionsService::getOptionValue( const Char * _key ) const
    {
        for( const Option & op : m_options )
        {
            if( strcmp( op.key, _key ) != 0 )
            {
                continue;
            }

            return op.value;
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OptionsService::getOptionUInt32( const Char * _key ) const
    {
        for( const Option & op : m_options )
        {
            if( strcmp( op.key, _key ) != 0 )
            {
                continue;
            }

            uint32_t value_uint32;
            if( ::sscanf( op.value, "%u", &value_uint32 ) != 1 )
            {
                LOGGER_ERROR( "option '%s' not sscanf value '%s'"
                    , _key
                    , op.value
                );

                return 0;
            }

            return value_uint32;
        }

        LOGGER_ERROR( "option '%s' not found"
            , _key
        );

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OptionsService::testOptionValue( const Char * _key, const Char * _value ) const
    {
        const Char * value = this->getOptionValue( _key );

        if( value == nullptr )
        {
            return false;
        }

        if( strcmp( value, _value ) != 0 )
        {
            return false;
        }

        return true;
    }
}