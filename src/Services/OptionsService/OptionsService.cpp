#include "OptionsService.h"

#include "Kernel/Logger.h"
#include "Kernel/Stringalized.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionCharacter.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/FactoryPool.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( OptionsService, Mengine::OptionsService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
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
        m_factoryOption = Helper::makeFactoryPool<Option, 16>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OptionsService::_finalizeService()
    {
        m_options.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryOption );
        m_factoryOption = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OptionsService::setArguments( const ArgumentsInterfacePtr & _arguments )
    {
        uint32_t argument_count = _arguments->getArgumentCount();

        for( uint32_t index = 0; index != argument_count; ++index )
        {
            const Char * option_str = _arguments->getArgument( index );

            const Char * option_key_str = StdString::strchr( option_str, '-' );

            if( option_key_str == nullptr )
            {
                continue;
            }

            ++option_key_str;

            if( *option_key_str == '-' )
            {
                ++option_key_str;
            }

            const Char * option_value_str = StdString::strchr( option_key_str, ':' );

            OptionPtr op = m_factoryOption->createObject( MENGINE_DOCUMENT_FACTORABLE );

            if( option_value_str == nullptr )
            {
                MENGINE_ASSERTION_FATAL( StdString::strlen( option_key_str ) < MENGINE_OPTIONS_KEY_SIZE, "option '%s' invalid key size %zu (max %u)"
                    , option_key_str
                    , StdString::strlen( option_key_str )
                    , MENGINE_OPTIONS_KEY_SIZE
                );

                op->setKey( option_key_str );
            }
            else
            {
                size_t key_size = option_value_str - option_key_str;

                MENGINE_ASSERTION_FATAL( key_size < MENGINE_OPTIONS_KEY_SIZE, "option '%s' invalid key size %zu (max %u)"
                    , option_key_str
                    , key_size
                    , MENGINE_OPTIONS_KEY_SIZE
                );

                op->setKey( option_key_str, key_size );

                const Char * option_delim_str = StdString::strchr( option_key_str, '|' );

                if( option_delim_str == nullptr )
                {
                    const Char * op_value = option_value_str + 1;

                    MENGINE_ASSERTION_FATAL( StdString::strlen( op_value ) < MENGINE_OPTIONS_VALUE_SIZE, "option '%s' invalid value size %zu (max %u)"
                        , op_value
                        , StdString::strlen( op_value )
                        , MENGINE_OPTIONS_VALUE_SIZE
                    );

                    op->addValue( op_value );
                }
                else
                {
                    for( ;;)
                    {
                        size_t value_size = option_delim_str - (option_value_str + 1);

                        MENGINE_ASSERTION_FATAL( value_size < MENGINE_OPTIONS_VALUE_SIZE, "option '%s' invalid value size %zu (max %u)"
                            , option_value_str + 1
                            , value_size
                            , MENGINE_OPTIONS_VALUE_SIZE
                        );

                        op->addValue( option_value_str + 1, value_size );

                        const Char * option_delim_test_str = StdString::strchr( option_delim_str + 1, '|' );

                        if( option_delim_test_str != nullptr )
                        {
                            option_delim_str = option_delim_test_str;

                            continue;
                        }

                        const Char * op_value = option_delim_str + 1;

                        MENGINE_ASSERTION_FATAL( StdString::strlen( op_value ) < MENGINE_OPTIONS_VALUE_SIZE, "option '%s' invalid value size %zu (max %u)"
                            , op_value
                            , StdString::strlen( op_value )
                            , MENGINE_OPTIONS_VALUE_SIZE
                        );

                        op->addValue( op_value );

                        break;
                    }
                }
            }

            const Char * optionKey = op->getKey();
            size_t optionKeySize = StdString::strlen( optionKey );

            MENGINE_ASSERTION_LOWER_CHARACTER_SET( optionKey, optionKeySize );

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
            uint32_t valueCount = op->getValueCount();

            for( uint32_t value_index = 0; value_index != valueCount; ++value_index )
            {
                const Char * value_str = op->getValue( value_index );
                size_t value_size = StdString::strlen( value_str );

                MENGINE_ASSERTION_LOWER_CHARACTER_SET( value_str, value_size );
            }
#endif

            constexpr size_t protectedSuffixSize = sizeof( "-token" ) - 1;

            bool protectedValue = optionKeySize >= protectedSuffixSize
                && StdString::strcmp( optionKey + optionKeySize - protectedSuffixSize, "-token" ) == 0;

            op->setProtected( protectedValue );

            m_options.push_back( op );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OptionsService::hasOption( const Char * _key, bool _withValue ) const
    {
        MENGINE_ASSERTION_LOWER_CHARACTER_SET( _key, StdString::strlen( _key ) );

        for( const OptionInterfacePtr & op : m_options )
        {
            if( StdString::strcmp( op->getKey(), _key ) != 0 )
            {
                continue;
            }

            if( _withValue == true && op->getValueCount() == 0 )
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
        MENGINE_ASSERTION_FATAL( StdString::strlen( _key ) < MENGINE_OPTIONS_KEY_SIZE, "option '%s' invalid key size %zu (max %u)"
            , _key
            , StdString::strlen( _key )
            , MENGINE_OPTIONS_KEY_SIZE
        );

        MENGINE_ASSERTION_FATAL( StdString::strlen( _value ) < MENGINE_OPTIONS_VALUE_SIZE, "option '%s' invalid value size %zu (max %u)"
            , _key
            , StdString::strlen( _value )
            , MENGINE_OPTIONS_VALUE_SIZE
        );

        MENGINE_ASSERTION_LOWER_CHARACTER_SET( _key, StdString::strlen( _key ) );
        MENGINE_ASSERTION_LOWER_CHARACTER_SET( _value, StdString::strlen( _value ) );

        for( const OptionInterfacePtr & op : m_options )
        {
            if( StdString::strcmp( op->getKey(), _key ) == 0 )
            {
                return false;
            }
        }

        OptionPtr op = m_factoryOption->createObject( MENGINE_DOCUMENT_FACTORABLE );
        op->setKey( _key );

        size_t valueSize = StdString::strlen( _value );

        if( valueSize != 0 )
        {
            op->addValue( _value );
        }

        constexpr size_t protectedSuffixSize = sizeof( "-token" ) - 1;

        size_t keySize = StdString::strlen( _key );
        bool protectedValue = keySize >= protectedSuffixSize
            && StdString::strcmp( _key + keySize - protectedSuffixSize, "-token" ) == 0;

        op->setProtected( protectedValue );

        m_options.push_back( op );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * OptionsService::getOptionValue( const Char * _key, const Char * _default ) const
    {
        MENGINE_ASSERTION_LOWER_CHARACTER_SET( _key, StdString::strlen( _key ) );

        for( const OptionInterfacePtr & op : m_options )
        {
            if( StdString::strcmp( op->getKey(), _key ) != 0 )
            {
                continue;
            }

            const Char * value_str = op->getValue( 0 );

            return value_str;
        }

        return _default;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OptionsService::getOptionValues( const Char * _key, const Char ** _values, uint32_t * const _count ) const
    {
        MENGINE_ASSERTION_LOWER_CHARACTER_SET( _key, StdString::strlen( _key ) );

        for( const OptionInterfacePtr & op : m_options )
        {
            if( StdString::strcmp( op->getKey(), _key ) != 0 )
            {
                continue;
            }

            uint32_t valueCount = op->getValueCount();

            for( uint32_t index = 0; index != valueCount; ++index )
            {
                _values[index] = op->getValue( index );
            }

            *_count = valueCount;

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t OptionsService::getOptionInt32( const Char * _key, int32_t _default ) const
    {
        MENGINE_ASSERTION_LOWER_CHARACTER_SET( _key, StdString::strlen( _key ) );

        for( const OptionInterfacePtr & op : m_options )
        {
            if( StdString::strcmp( op->getKey(), _key ) != 0 )
            {
                continue;
            }

            const Char * value = op->getValue( 0 );

            int32_t value_int32;
            if( Helper::stringalized( value, &value_int32 ) == false )
            {
                LOGGER_ERROR( "option '%s' invalid cast to int32_t value '%s'"
                    , _key
                    , value
                );

                return 0;
            }

            return value_int32;
        }

        return _default;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OptionsService::getOptionUInt32( const Char * _key, uint32_t _default ) const
    {
        MENGINE_ASSERTION_LOWER_CHARACTER_SET( _key, StdString::strlen( _key ) );

        for( const OptionInterfacePtr & op : m_options )
        {
            if( StdString::strcmp( op->getKey(), _key ) != 0 )
            {
                continue;
            }

            const Char * value = op->getValue( 0 );

            uint32_t value_uint32;
            if( Helper::stringalized( value, &value_uint32 ) == false )
            {
                LOGGER_ERROR( "option '%s' invalid cast to uint32_t value '%s'"
                    , _key
                    , value
                );

                return MENGINE_UINT32_C(0);
            }

            return value_uint32;
        }

        return _default;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OptionsService::testOptionValue( const Char * _key, const Char * _value ) const
    {
        MENGINE_ASSERTION_LOWER_CHARACTER_SET( _key, StdString::strlen( _key ) );
        MENGINE_ASSERTION_LOWER_CHARACTER_SET( _value, StdString::strlen( _value ) );

        if( this->hasOption( _key, true ) == false )
        {
            return false;
        }

        const Char * value = this->getOptionValue( _key, "" );

        if( StdString::strcmp( value, _value ) != 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OptionsService::foreachOptions( const LambdaOption & _lambda ) const
    {
        for( const OptionInterfacePtr & option : m_options )
        {
            _lambda( option );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
