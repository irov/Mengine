#pragma once

#include "Kernel/ConstString.h"

namespace Mengine
{
    namespace Detail
    {
        void assertionVocabularyEmpty( const ConstString & _category, const Char * _file, uint32_t _line );
    }
}

#ifdef MENGINE_ASSERTION_DEBUG
#   define MENGINE_ASSERTION_VOCABULARY_EMPTY(Category) Detail::assertionVocabularyEmpty(Category, MENGINE_CODE_FILE, MENGINE_CODE_LINE)
#else
#   define MENGINE_ASSERTION_VOCABULARY_EMPTY(Category)
#endif

