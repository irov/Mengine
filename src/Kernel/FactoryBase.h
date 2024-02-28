#pragma once

#include "Config/Typedef.h"

#include "Interface/ServiceInterface.h"
#include "Interface/FactoryInterface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/FactorablePointer.h"
#include "Kernel/FactorableUnique.h"

#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Pointer.h"
#include "Kernel/ThreadGuard.h"

#include "Kernel/ReferenceCounter.h"

#if defined(MENGINE_DEBUG)
#   include "Kernel/IntrusiveList.h"
#endif

namespace Mengine
{    
    class FactoryBase
        : public FactoryInterface
    {
    public:
        FactoryBase();
        ~FactoryBase() override;

    public:
        void setType( const ConstString & _type ) override;
        const ConstString & getType() const override;

    public:
        bool isEmptyObjects() const override;
        uint32_t getCountObject() const override;

    protected:
        virtual Factorable * _createObject() = 0;
        virtual void _destroyObject( Factorable * _object ) = 0;

#if defined(MENGINE_DEBUG)
    protected:
        void foreachFactorables( const LambdaFactorable & _lambda ) override;
#endif

    protected:
        ConstString m_type;

        ReferenceCounter m_count;

#if defined(MENGINE_DEBUG)
        typedef IntrusiveList<Factorable> IntrusiveListFactorables;
        IntrusiveListFactorables m_factorables;
#endif

#if defined(MENGINE_DEBUG)
        bool m_register;
#endif
    };
}
