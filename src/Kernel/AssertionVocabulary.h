#pragma once

#include "Kernel/Assertion.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Kernel/ConstString.h"

#   include "Config/Char.h"
#   include "Config/Typedef.h"

namespace Mengine
{
    namespace Detail
    {
        void assertionVocabularyEmpty( const Char * _category, const ConstString & _key, const Char * _file, int32_t _line, const Char * _function );
    }
}

#   define MENGINE_ASSERTION_VOCABULARY_EMPTY(Key) Mengine::Detail::assertionVocabularyEmpty(MENGINE_CODE_LIBRARY, Key, MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION)
#else
#   define MENGINE_ASSERTION_VOCABULARY_EMPTY(Key)
#endif
