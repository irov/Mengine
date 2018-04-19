#pragma once

#include "Interface/ServiceInterface.h"

#include "Factory/FactorablePtr.h"
#include "Factory/FactorablePointer.h"
#include "Factory/FactorableUnique.h"

#include "Core/Assertion.h"
#include "Core/Pointer.h"

#include "stdex/thread_guard.h"

#include <stddef.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Factory
        : public FactorableUnique<FactorablePtr>
    {
    public:
        Factory( const Char * _name );
        virtual ~Factory();

    public:
        virtual PointerFactorable createObject();
        virtual void destroyObject( Factorable * _object );

    public:
        bool isEmptyObjects() const;
        uint32_t getCountObject() const;

    protected:
        virtual Factorable * _createObject() = 0;
        virtual void _destroyObject( Factorable * _object ) = 0;

    protected:
        void _destroy() override;
        void destroy() override;

    protected:
        const Char * m_name;

        uint32_t m_count;

        STDEX_THREAD_GUARD_INIT;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<Factory> FactoryPtr;
    //////////////////////////////////////////////////////////////////////////
#   define MENGINE_ASSERTION_FACTORY_EMPTY(F) MENGINE_ASSERTION((F) == nullptr || (F)->isEmptyObjects() == true)
    //////////////////////////////////////////////////////////////////////////
}
