#pragma once

#include "Environment/Android/AndroidIncluder.h"

namespace Mengine
{
    namespace Helper
    {
        class JClassDefinition
        {
        public:
            static jclass OBJECT;
            static jclass BOOLEAN;
            static jclass CHARACTER;
            static jclass INTEGER;
            static jclass LONG;
            static jclass FLOAT;
            static jclass DOUBLE;
            static jclass STRING;
            static jclass ARRAY_LIST;
            static jclass MAP;
            static jclass HASH_MAP;
            static jclass SET;
            static jclass ITERATOR;
            static jclass MAP_ENTRY;
        };
    }
}
