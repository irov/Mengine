#pragma once

#include "Config/Typedef.h"

#include "Interface/ServiceInterface.h"

#include "Kernel/FactorablePointer.h"
#include "Kernel/FactorableUnique.h"

#include "Kernel/Assertion.h"
#include "Kernel/Pointer.h"

#include "stdex/thread_guard.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Factory
        : public FactorableUnique<Factorable>
    {
    public:
        Factory( const Char * _name );
        virtual ~Factory();

    public:
        const Char * getName() const;

    public:
        virtual FactorablePointer createObject();
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
    typedef IntrusivePtr<Factory> FactoryPtr;
    //////////////////////////////////////////////////////////////////////////
}
