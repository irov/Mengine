#include "OptionsService.h"

#include "Kernel/Logger.h"

#include "Config/StdString.h"

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
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OptionsService::_finalizeService()
    {
        m_options.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool OptionsService::setArguments( const ArgumentsInterfacePtr & _arguments )
    {
        uint32_t argument_count = _arguments->getArgumentCount();

        for( uint32_t index = 0; index != argument_count; ++index )
        {
            const Char * option_str = _arguments->getArgument( index );

            const Char * option_key_str = MENGINE_STRCHR( option_str, '-' );

            if( option_key_str == nullptr )
            {
                continue;
            }

            option_key_str += 1;

            const Char * option_value_str = MENGINE_STRCHR( option_key_str, ':' );

            Option op;

            if( option_value_str == nullptr )
            {
                if( MENGINE_STRLEN( option_key_str ) >= MENGINE_OPTIONS_KEY_SIZE )
                {
                    return false;
                }

                MENGINE_STRCPY( op.key, option_key_str );

                op.value[0] = '\0';
            }
            else
            {
                size_t key_size = option_value_str - option_key_str;

                if( key_size >= MENGINE_OPTIONS_KEY_SIZE )
                {
                    return false;
                }

                MENGINE_STRNCPY( op.key, option_key_str, key_size );
                op.key[key_size] = '\0';

                if( MENGINE_STRLEN( option_value_str + 1 ) >= MENGINE_OPTIONS_VALUE_SIZE )
                {
                    return false;
                }

                MENGINE_STRCPY( op.value, option_value_str + 1 );
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
            if( MENGINE_STRCMP( op.key, _key ) != 0 )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * OptionsService::getOptionValue( const Char * _key, const Char * _default ) const
    {
        for( const Option & op : m_options )
        {
            if( MENGINE_STRCMP( op.key, _key ) != 0 )
            {
                continue;
            }

            return op.value;
        }

        if( _default == nullptr )
        {
            LOGGER_ERROR( "option '%s' not found"
                , _key
            );

            return nullptr;
        }

        return _default;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OptionsService::getOptionUInt32( const Char * _key ) const
    {
        for( const Option & op : m_options )
        {
            if( MENGINE_STRCMP( op.key, _key ) != 0 )
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
        if( this->hasOption( _key ) == false )
        {
            return false;
        }

        const Char * value = this->getOptionValue( _key, "" );

        if( MENGINE_STRCMP( value, _value ) != 0 )
        {
            return false;
        }

        return true;
    }
}