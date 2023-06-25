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
        void assertionStandardCharacterSet( const Char * _category, const Char * _value, size_t _len, const Char * _file, uint32_t _line )
        {
            for( const Char * ch_iterator = _value; ch_iterator != _value + _len; ++ch_iterator )
            {
                Char ch = *ch_iterator;

                if( Helper::isStandardCharacterSet( ch ) == true )
                {
                    continue;
                }

                if( Helper::isDigitCharacterSet( ch ) == true )
                {
                    continue;
                }

                Char msg[MENGINE_ASSERTION_MAX_MESSAGE] = {'\0'};
                MENGINE_SNPRINTF( msg, MENGINE_ASSERTION_MAX_MESSAGE, "[Assert] string '%.*s' has non standard char '%c' code: %u"
                    , (int32_t)_len
                    , _value
                    , ch
                    , (uint32_t)ch
                );

                Mengine::Helper::Assertion( _category, ASSERTION_LEVEL_ERROR, msg, _file, _line, "Assertion Standard Character Set" );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void assertionLowerCharacterSet( const Char * _category, const Char * _value, size_t _len, const Char * _file, uint32_t _line )
        {
            for( const Char * ch_iterator = _value; ch_iterator != _value + _len; ++ch_iterator )
            {
                Char ch = *ch_iterator;

                if( Helper::isLowerCharacterSet( ch ) == true )
                {
                    continue;
                }

                if( Helper::isDigitCharacterSet( ch ) == true )
                {
                    continue;
                }

                Char msg[MENGINE_ASSERTION_MAX_MESSAGE] = {'\0'};
                MENGINE_SNPRINTF( msg, MENGINE_ASSERTION_MAX_MESSAGE, "[Assert] string '%.*s' has non lower char '%c' code: %u"
                    , (int32_t)_len
                    , _value
                    , ch
                    , (uint32_t)ch
                );

                Mengine::Helper::Assertion( _category, ASSERTION_LEVEL_ERROR, msg, _file, _line, "Assertion Lower Character Set" );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void assertionUpperCharacterSet( const Char * _category, const Char * _value, size_t _len, const Char * _file, uint32_t _line )
        {
            for( const Char * ch_iterator = _value; ch_iterator != _value + _len; ++ch_iterator )
            {
                Char ch = *ch_iterator;

                if( Helper::isUpperCharacterSet( ch ) == true )
                {
                    continue;
                }

                if( Helper::isDigitCharacterSet( ch ) == true )
                {
                    continue;
                }

                Char msg[MENGINE_ASSERTION_MAX_MESSAGE] = {'\0'};
                MENGINE_SNPRINTF( msg, MENGINE_ASSERTION_MAX_MESSAGE, "[Assert] string '%.*s' has non upper char '%c' code: %u"
                    , (int32_t)_len
                    , _value
                    , ch
                    , (uint32_t)ch
                );

                Mengine::Helper::Assertion( _category, ASSERTION_LEVEL_ERROR, msg, _file, _line, "Assertion Upper Character Set" );
            }
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
#endif