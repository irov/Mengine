#include "OptionsService.h"

#include "Interface/LoggerServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Stringalized.h"

#include "Config/StdString.h"

#include <algorithm>

#include <ctype.h>

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
        SERVICE_WAIT( LoggerServiceInterface, [this]()
        {
            for( const Option & option : m_options )
            {
                if( option.value_count == 0 )
                {
                    LOGGER_MESSAGE_RELEASE( "option: -%s"
                        , option.key
                    );
                }
                else
                {
                    if( option.value_count == 1 )
                    {
                        LOGGER_MESSAGE_RELEASE( "option: -%s=%s"
                            , option.key
                            , option.value[0]
                        );
                    }
                    else
                    {
                        LOGGER_MESSAGE_RELEASE_WN( "option: -%s="
                            , option.key
                        );

                        for( uint32_t index = 0; index != option.value_count; ++index )
                        {
                            LOGGER_MESSAGE_RELEASE_WN( "%s|"
                                , option.value[index]
                            );
                        }

                        LOGGER_MESSAGE_RELEASE( "\n" );
                    }
                }
            }

            return true;
        } );

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

            ++option_key_str;

            if( *option_key_str == '-' )
            {
                ++option_key_str;
            }

            const Char * option_value_str = MENGINE_STRCHR( option_key_str, ':' );

            Option op;

            if( option_value_str == nullptr )
            {
                if( MENGINE_STRLEN( option_key_str ) >= MENGINE_OPTIONS_KEY_SIZE )
                {
                    return false;
                }

                MENGINE_STRCPY( op.key, option_key_str );

                op.value[0][0] = '\0';
                op.value_count = 0;
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

                const Char * option_delim_str = MENGINE_STRCHR( option_key_str, '|' );

                if( option_delim_str == nullptr )
                {
                    if( MENGINE_STRLEN( option_value_str + 1 ) >= MENGINE_OPTIONS_VALUE_SIZE )
                    {
                        return false;
                    }

                    MENGINE_STRCPY( op.value[0], option_value_str + 1 );
                    op.value_count = 1;
                }
                else
                {
                    op.value_count = 0;

                    for( ;;)
                    {
                        size_t value_size = option_delim_str - (option_value_str + 1);

                        if( value_size >= MENGINE_OPTIONS_VALUE_SIZE )
                        {
                            return false;
                        }

                        MENGINE_STRNCPY( op.value[op.value_count], option_value_str + 1, value_size );
                        ++op.value_count;

                        const Char * option_delim_test_str = MENGINE_STRCHR( option_delim_str + 1, '|' );

                        if( option_delim_test_str != nullptr )
                        {
                            option_delim_str = option_delim_test_str;

                            continue;
                        }

                        if( MENGINE_STRLEN( option_delim_str + 1 ) >= MENGINE_OPTIONS_VALUE_SIZE )
                        {
                            return false;
                        }

                        MENGINE_STRCPY( op.value[op.value_count], option_delim_str + 1 );
                        ++op.value_count;

                        break;
                    }
                        
                }
            }

            MENGINE_ASSERTION_FATAL( std::count_if( op.key, op.key + MENGINE_STRLEN( op.key ), []( Char _ch )
            {
                return ::isupper( _ch ) != 0;
            } ) == 0 );

            m_options.push_back( op );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OptionsService::hasOption( const Char * _key ) const
    {
        MENGINE_ASSERTION_FATAL( std::count_if( _key, _key + MENGINE_STRLEN( _key ), []( Char _ch )
        {
            return ::isupper( _ch ) != 0;
        } ) == 0 );

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
        MENGINE_ASSERTION_FATAL( std::count_if( _key, _key + MENGINE_STRLEN( _key ), []( Char _ch )
        {
            return ::isupper( _ch ) != 0;
        } ) == 0 );

        for( const Option & op : m_options )
        {
            if( MENGINE_STRCMP( op.key, _key ) != 0 )
            {
                continue;
            }

            return op.value[0];
        }

        return _default;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OptionsService::getOptionValues( const Char * _key, const Char ** _values, uint32_t * _count ) const
    {
        MENGINE_ASSERTION_FATAL( std::count_if( _key, _key + MENGINE_STRLEN( _key ), []( Char _ch )
        {
            return std::isupper( _ch ) != 0;
        } ) == 0 );

        for( const Option & op : m_options )
        {
            if( MENGINE_STRCMP( op.key, _key ) != 0 )
            {
                continue;
            }

            for( uint32_t index = 0; index != op.value_count; ++index )
            {
                _values[index] = op.value[index];
            }

            *_count = op.value_count;

            return true;
        }

        *_count = 0;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OptionsService::getOptionUInt32( const Char * _key, uint32_t _default ) const
    {
        MENGINE_ASSERTION_FATAL( std::count_if( _key, _key + MENGINE_STRLEN( _key ), []( Char _ch )
        {
            return ::isupper( _ch ) != 0;
        } ) == 0 );

        for( const Option & op : m_options )
        {
            if( MENGINE_STRCMP( op.key, _key ) != 0 )
            {
                continue;
            }

            uint32_t value_uint32;
            if( Helper::stringalized( op.value[0], &value_uint32 ) == false )
            {
                LOGGER_ERROR( "option '%s' invalid cast to uint32_t value '%s'"
                    , _key
                    , op.value
                );

                return 0;
            }

            return value_uint32;
        }

        return _default;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OptionsService::testOptionValue( const Char * _key, const Char * _value ) const
    {
        MENGINE_ASSERTION_FATAL( std::count_if( _key, _key + MENGINE_STRLEN( _key ), []( Char _ch )
        {
            return ::isupper( _ch ) != 0;
        } ) == 0 );

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