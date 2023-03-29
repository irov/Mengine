#pragma once

#include "Kernel/IntrusivePtr.h"
#include "Kernel/Document.h"
#include "Kernel/MemoryAllocator.h"
#include "Kernel/AssertionMemoryPanic.h"

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
        IntrusivePtr<Type> makeFactorableUnique( const DocumentPtr & _doc, Args && ... _args )
        {
            MENGINE_UNUSED( _doc );

            Type * factorable = Helper::newT<FactorableUnique<Type>>( std::forward<Args &&>( _args ) ... );

            MENGINE_ASSERTION_MEMORY_PANIC( factorable );

#if defined(MENGINE_DOCUMENT_ENABLE)
            factorable->setDocument( _doc );
#endif

            return IntrusivePtr<Type>( factorable );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
