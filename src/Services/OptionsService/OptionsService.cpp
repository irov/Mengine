#include "OptionsService.h"

#include "Interface/LoggerServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Stringalized.h"
#include "Kernel/Assertion.h"
#include "Kernel/Stringstream.h"
#include "Kernel/AssertionCharacter.h"

#include "Config/StdString.h"

#include "Config/Algorithm.h"

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
        SERVICE_WAIT_METHOD( LoggerServiceInterface, this, OptionsService::logOptions_ );

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

                op.key.assign( option_key_str );

                op.value[0].clear();
                op.value_count = 0;
            }
            else
            {
                size_t key_size = option_value_str - option_key_str;

                if( key_size >= MENGINE_OPTIONS_KEY_SIZE )
                {
                    return false;
                }

                op.key.assign( option_key_str, key_size );

                const Char * option_delim_str = MENGINE_STRCHR( option_key_str, '|' );

                if( option_delim_str == nullptr )
                {
                    const Char * op_value = option_value_str + 1;

                    if( MENGINE_STRLEN( op_value ) >= MENGINE_OPTIONS_VALUE_SIZE )
                    {
                        return false;
                    }

                    op.value[0].append( op_value );
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

                        op.value[op.value_count].append( option_value_str + 1, value_size );
                        ++op.value_count;

                        const Char * option_delim_test_str = MENGINE_STRCHR( option_delim_str + 1, '|' );

                        if( option_delim_test_str != nullptr )
                        {
                            option_delim_str = option_delim_test_str;

                            continue;
                        }

                        const Char * op_value = option_delim_str + 1;

                        if( MENGINE_STRLEN( op_value ) >= MENGINE_OPTIONS_VALUE_SIZE )
                        {
                            return false;
                        }

                        op.value[op.value_count].append( op_value );
                        ++op.value_count;

                        break;
                    }

                }
            }

            MENGINE_ASSERTION_LOWER_CHARACTER_SET( op.key.c_str(), op.key.size() );

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
            for( uint32_t value_index = 0; value_index != op.value_count; ++value_index )
            {
                const Char * value_str = op.value[value_index].c_str();
                size_t value_size = op.value[value_index].size();

                MENGINE_ASSERTION_LOWER_CHARACTER_SET( value_str, value_size );
            }
#endif

            m_options.push_back( op );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OptionsService::hasOption( const Char * _key, bool _withValue ) const
    {
        MENGINE_ASSERTION_LOWER_CHARACTER_SET( _key, MENGINE_STRLEN( _key ) );

        for( const Option & op : m_options )
        {
            if( op.key.compare( _key ) != 0 )
            {
                continue;
            }

            if( _withValue == true && op.value_count == 0 )
            {
                return false;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OptionsService::setOptionValue( const Char * _key, const Char * _value )
    {
        MENGINE_ASSERTION_FATAL( MENGINE_STRLEN( _key ) < MENGINE_OPTIONS_KEY_SIZE );
        MENGINE_ASSERTION_FATAL( MENGINE_STRLEN( _value ) < MENGINE_OPTIONS_VALUE_SIZE );

        MENGINE_ASSERTION_LOWER_CHARACTER_SET( _key, MENGINE_STRLEN( _key ) );
        MENGINE_ASSERTION_LOWER_CHARACTER_SET( _value, MENGINE_STRLEN( _value ) );

        for( const Option & op : m_options )
        {
            if( op.key.compare( _key ) == 0 )
            {
                return false;
            }
        }

        Option op;
        op.key.assign( _key );

        if( MENGINE_STRLEN( _value ) == 0 )
        {
            op.value[0].clear();
            op.value_count = 0;
        }
        else
        {
            op.value[0].append( _value );
            
            op.value_count = 1;
        }        

        m_options.push_back( op );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * OptionsService::getOptionValue( const Char * _key, const Char * _default ) const
    {
        MENGINE_ASSERTION_LOWER_CHARACTER_SET( _key, MENGINE_STRLEN( _key ) );

        for( const Option & op : m_options )
        {
            if( op.key.compare( _key ) != 0 )
            {
                continue;
            }

            return op.value[0].c_str();
        }

        return _default;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OptionsService::getOptionValues( const Char * _key, const Char ** _values, uint32_t * const _count ) const
    {
        MENGINE_ASSERTION_LOWER_CHARACTER_SET( _key, MENGINE_STRLEN( _key ) );

        for( const Option & op : m_options )
        {
            if( op.key.compare( _key ) != 0 )
            {
                continue;
            }

            for( uint32_t index = 0; index != op.value_count; ++index )
            {
                _values[index] = op.value[index].c_str();
            }

            *_count = op.value_count;

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OptionsService::getOptionUInt32( const Char * _key, uint32_t _default ) const
    {
        MENGINE_ASSERTION_LOWER_CHARACTER_SET( _key, MENGINE_STRLEN( _key ) );

        for( const Option & op : m_options )
        {
            if( op.key.compare( _key ) != 0 )
            {
                continue;
            }

            uint32_t value_uint32;
            if( Helper::stringalized( op.value[0].c_str(), &value_uint32 ) == false )
            {
                LOGGER_ERROR( "option '%s' invalid cast to uint32_t value '%s'"
                    , _key
                    , op.value[0].c_str()
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
        MENGINE_ASSERTION_LOWER_CHARACTER_SET( _key, MENGINE_STRLEN( _key ) );
        MENGINE_ASSERTION_LOWER_CHARACTER_SET( _value, MENGINE_STRLEN( _value ) );

        if( this->hasOption( _key, true ) == false )
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
    //////////////////////////////////////////////////////////////////////////
    bool OptionsService::logOptions_()
    {
        Stringstream ss;

        ss << "options:";

        if( m_options.empty() == true )
        {
            ss << " [none]";
        }

        for( const Option & option : m_options )
        {
            if( option.value_count == 0 )
            {
                ss << " -" << option.key.c_str();
            }
            else
            {
                if( option.value_count == 1 )
                {
                    ss << " -" << option.key.c_str() << "=" << option.value[0].c_str();
                }
                else
                {
                    ss << " -" << option.key.c_str() << "=";

                    for( uint32_t index = 0; index != option.value_count; ++index )
                    {
                        ss << option.value[index].c_str() << "|";
                    }
                }
            }
        }

        LOGGER_MESSAGE( "%s"
            , ss.str().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
