#pragma once

#include "Kernel/ConstString.h"

namespace Mengine
{
    namespace Detail
    {
        void assertionVocabularyEmpty( const ConstString & _category, const Char * _file, uint32_t _line );
    }
}

#ifdef MENGINE_DEBUG
#   define MENGINE_ASSERTION_VOCABULARY_EMPTY(Category) Detail::assertionVocabularyEmpty(Category, __FILE__, __LINE__)
#else
#   define MENGINE_ASSERTION_VOCABULARY_EMPTY(Category)
#endif

