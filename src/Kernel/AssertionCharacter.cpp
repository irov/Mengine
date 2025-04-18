#include "AssertionCharacter.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Kernel/CharacterHelper.h"

#   include "Kernel/Assertion.h"
#   include "Kernel/Logger.h"

#   include "Config/StdIO.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        void assertionStandardCharacterSet( const Char * _category, const Char * _value, size_t _len, const Char * _file, int32_t _line, const Char * _function )
        {
            for( const Char * ch_iterator = _value; ch_iterator != _value + _len; ++ch_iterator )
            {
                Char ch = *ch_iterator;

                if( Helper::isASCIICharacterSet( ch ) == false )
                {
                    Char msg[MENGINE_ASSERTION_MAX_MESSAGE + 1] = {'\0'};
                    MENGINE_SNPRINTF( msg, MENGINE_ASSERTION_MAX_MESSAGE, "string '%.*s' has non ASCII char code: %u"
                        , (int32_t)_len
                        , _value
                        , (uint32_t)ch
                    );

                    Helper::Assertion( _category, ASSERTION_LEVEL_ERROR, msg, _file, _line, _function, "Assertion Standard Character Set [No ASCII]" );
                }

                if( Helper::isStandardCharacterSet( ch ) == false )
                {
                    Char msg[MENGINE_ASSERTION_MAX_MESSAGE + 1] = {'\0'};
                    MENGINE_SNPRINTF( msg, MENGINE_ASSERTION_MAX_MESSAGE, "string '%.*s' has non standard char '%c' code: %u"
                        , (int32_t)_len
                        , _value
                        , ch
                        , (uint32_t)ch
                    );

                    Helper::Assertion( _category, ASSERTION_LEVEL_ERROR, msg, _file, _line, _function, "Assertion Standard Character Set" );
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void assertionLowerCharacterSet( const Char * _category, const Char * _value, size_t _len, const Char * _file, int32_t _line, const Char * _function )
        {
            for( const Char * ch_iterator = _value; ch_iterator != _value + _len; ++ch_iterator )
            {
                Char ch = *ch_iterator;

                if( Helper::isASCIICharacterSet( ch ) == false )
                {
                    Char msg[MENGINE_ASSERTION_MAX_MESSAGE + 1] = {'\0'};
                    MENGINE_SNPRINTF( msg, MENGINE_ASSERTION_MAX_MESSAGE, "string '%.*s' has non ASCII char code: %u"
                        , (int32_t)_len
                        , _value
                        , (uint32_t)ch
                    );

                    Helper::Assertion( _category, ASSERTION_LEVEL_ERROR, msg, _file, _line, _function, "Assertion Lower Character Set" );
                }

                if( Helper::isUpperCharacterSet( ch ) == true )
                {
                    Char msg[MENGINE_ASSERTION_MAX_MESSAGE + 1] = {'\0'};
                    MENGINE_SNPRINTF( msg, MENGINE_ASSERTION_MAX_MESSAGE, "string '%.*s' has non lower char '%c' code: %u"
                        , (int32_t)_len
                        , _value
                        , ch
                        , (uint32_t)ch
                    );

                    Helper::Assertion( _category, ASSERTION_LEVEL_ERROR, msg, _file, _line, _function, "Assertion Lower Character Set" );
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void assertionUpperCharacterSet( const Char * _category, const Char * _value, size_t _len, const Char * _file, int32_t _line, const Char * _function )
        {
            for( const Char * ch_iterator = _value; ch_iterator != _value + _len; ++ch_iterator )
            {
                Char ch = *ch_iterator;

                if( Helper::isASCIICharacterSet( ch ) == false )
                {
                    Char msg[MENGINE_ASSERTION_MAX_MESSAGE + 1] = {'\0'};
                    MENGINE_SNPRINTF( msg, MENGINE_ASSERTION_MAX_MESSAGE, "string '%.*s' has non ASCII char code: %u"
                        , (int32_t)_len
                        , _value
                        , (uint32_t)ch
                    );

                    Helper::Assertion( _category, ASSERTION_LEVEL_ERROR, msg, _file, _line, _function, "Assertion Upper Character Set" );
                }

                if( Helper::isLowerCharacterSet( ch ) == true )
                {
                    Char msg[MENGINE_ASSERTION_MAX_MESSAGE + 1] = {'\0'};
                    MENGINE_SNPRINTF( msg, MENGINE_ASSERTION_MAX_MESSAGE, "string '%.*s' has non upper char '%c' code: %u"
                        , (int32_t)_len
                        , _value
                        , ch
                        , (uint32_t)ch
                    );

                    Mengine::Helper::Assertion( _category, ASSERTION_LEVEL_ERROR, msg, _file, _line, _function, "Assertion Upper Character Set" );
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
#endif
