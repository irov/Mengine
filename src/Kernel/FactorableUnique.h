#pragma once

#include "Interface/DocumentInterface.h"

#include "Kernel/IntrusivePtr.h"
#include "Kernel/MemoryAllocator.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/EnumeratorHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class Base>
    class FactorableUnique
        : public Base
    {
    public:
        template<class ... Args>
        FactorableUnique( Args && ... _args )
            : Base( std::forward<Args &&>( _args ) ... )
        {
        }

        ~FactorableUnique() override
        {
        }

    public:
        void destroy() override
        {
            this->_destroy();

            Helper::deleteT( this );
        }
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class Type, class ... Args>
        IntrusivePtr<Type> makeFactorableUnique( const DocumentInterfacePtr & _doc, Args && ... _args )
        {
            MENGINE_UNUSED( _doc );

            Type * factorable = Helper::newT<FactorableUnique<Type>>( std::forward<Args>( _args ) ... );

            MENGINE_ASSERTION_MEMORY_PANIC( factorable, "invalid create factorable '%s'"
                , Typename<Type>::value
            );

            UniqueId id = Helper::generateUniqueIdentity();
            factorable->setUniqueIdentity( id );

#if defined(MENGINE_DOCUMENT_ENABLE)
            factorable->setDocument( _doc );
#endif

            return IntrusivePtr<Type>::from( factorable );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
