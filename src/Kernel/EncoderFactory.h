#pragma once

#include "Interface/FactoryInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/FactoryDefault.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        MENGINE_INLINE FactoryInterfacePtr registerEncoder( const ConstString & _type, const DocumentInterfacePtr & _doc )
        {
            FactoryInterfacePtr factory = Helper::makeFactoryDefault<T>( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( factory, "invalid register encoder '%s'"
                , _type.c_str()
            );

            VOCABULARY_SET( FactoryInterface, STRINGIZE_STRING_LOCAL( "EncoderFactory" ), _type, factory, _doc );

            return factory;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE void unregisterEncoder( const ConstString & _type )
        {
            FactoryInterfacePtr factory = VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "EncoderFactory" ), _type );

            MENGINE_ASSERTION_MEMORY_PANIC( factory, "invalid unregister encoder '%s'"
                , _type.c_str()
            );

            MENGINE_UNUSED( factory );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}