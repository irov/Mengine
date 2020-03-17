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
        ~Factory() override;

    public:
        const Char * getName() const;

    public:
        virtual FactorablePointer createObject( const DocumentPtr & _doc );
        virtual void destroyObject( Factorable * _object );

    public:
        bool isEmptyObjects() const;
        uint32_t getCountObject() const;

    protected:
        virtual Factorable * _createObject() = 0;
        virtual void _destroyObject( Factorable * _object ) = 0;

    protected:
        void destroy() override;

    protected:
        const Char * m_name;

        uint32_t m_count;

#ifdef MENGINE_DEBUG
        bool m_register;
#endif

        STDEX_THREAD_GUARD_INIT;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Factory> FactoryPtr;
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class T, class ... Args>
        FactoryPtr makeFactory( Args && ... _args )
        {
            return FactoryPtr( new T( std::forward<Args>( _args ) ... ) );
        }
    }
}
