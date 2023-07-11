#include "AssertionVocabulary.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Interface/VocabularyServiceInterface.h"

#   include "Kernel/Assertion.h"
#   include "Kernel/Logger.h"

#   include "Config/StdIO.h"

namespace Mengine
{
    namespace Detail
    {
        void assertionVocabularyEmpty( const Char * _category, const ConstString & _key, const Char * _file, uint32_t _line )
        {
            uint32_t count = 0;

            VOCABULARY_SERVICE()
                ->foreachVocabulary( [filter_key = _key, &count]( const ConstString & _key, const ConstString & _type, const MixinPtr & _mixin )
            {
                MENGINE_UNUSED( _type );
                MENGINE_UNUSED( _mixin );

                if( filter_key != _key )
                {
                    return;
                }

                LOGGER_ASSERTION( "vocabulary '%s' type '%s'"
                    , _key.c_str()
                    , _type.c_str()
                );

                ++count;
            } );

            if( count == 0 )
            {
                return;
            }

            Char msg[1024] = {'\0'};
            MENGINE_SNPRINTF( msg, 1024, "[Assert] Vocabulary '%s' not empty ['%u']"
                , _key.c_str()
                , count
            );

            Mengine::Helper::Assertion( _category, ASSERTION_LEVEL_ERROR, msg, _file, _line, "Assertion Vocabulary Empty" );
        }
    }
}
#endif