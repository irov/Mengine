#pragma once

#include "Config/Config.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Kernel/ConstString.h"

#   include "Config/Char.h"
#   include "Config/Typedef.h"

namespace Mengine
{
    namespace Detail
    {
        void assertionVocabularyEmpty( const ConstString & _category, const Char * _file, uint32_t _line );
    }
}

#   define MENGINE_ASSERTION_VOCABULARY_EMPTY(Category) Detail::assertionVocabularyEmpty(Category, MENGINE_CODE_FILE, MENGINE_CODE_LINE)
#else
#   define MENGINE_ASSERTION_VOCABULARY_EMPTY(Category)
#endif