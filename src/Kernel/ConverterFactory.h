#pragma once

#include "Interface/FactoryInterface.h"

#include "Kernel/ConstString.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/VocabularyHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        MENGINE_INLINE FactoryInterfacePtr registerConverter( const ConstString & _type, const DocumentInterfacePtr & _doc )
        {
            FactoryInterfacePtr factory = Helper::makeFactoryPool<T, 8>( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( factory, "invalid register converter '%s'"
                , _type.c_str()
            );

            VOCABULARY_SET( FactoryInterface, STRINGIZE_STRING_LOCAL( "ConverterFactory" ), _type, factory, _doc );

            return factory;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE void unregisterConverter( const ConstString & _type )
        {
            FactoryInterfacePtr factory = VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ConverterFactory" ), _type );

            MENGINE_ASSERTION_MEMORY_PANIC( factory, "invalid unregister decoder '%s'"
                , _type.c_str()
            );

            MENGINE_UNUSED( factory );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}