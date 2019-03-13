#include "Factory.h"

#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"

#include <stdlib.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        void assertionVocabularyEmpty( const ConstString & _category, const Char * _file, uint32_t _line )
        {
            MENGINE_UNUSED( _category );
            MENGINE_UNUSED( _file );
            MENGINE_UNUSED( _line );

#ifndef MENGINE_MASTER_RELEASE
            uint32_t count = 0;

            VOCABULARY_SERVICE()
                ->foreachVocabulary( _category, [_category, &count]( const ConstString & _type, const MixinPtr & _mixin )
            {
                MENGINE_UNUSED( _type );
                MENGINE_UNUSED( _mixin );

                LOGGER_ERROR( "Vocabulary '%s' type '%s'"
                    , _category.c_str()
                    , _type.c_str()
                );

                ++count;
            } );

            if( count == 0 )
            {
                return;
            }

            Char msg[1024];
            sprintf( msg, "[Assert] Vocabulary '%s' not empty ['%u']"
                , _category.c_str()
                , count
            );

            Mengine::Helper::Assertion( ASSERTION_LEVEL_ERROR, msg, _file, _line, "Assertion Vocabulary Empty" );
#endif
        }
    }
}