#pragma once

#include "Config/Typedef.h"

#include "Interface/ServiceInterface.h"

#include "Kernel/FactorablePointer.h"
#include "Kernel/FactorableUnique.h"

#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Document.h"
#include "Kernel/Pointer.h"

#include "Kernel/ReferenceCounter.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Factory
        : public Factorable
    {
    public:
        Factory();
        ~Factory() override;

    public:
        bool initialize( const ConstString & _type );
        void finalize();

    public:
        MENGINE_INLINE const ConstString & getType() const;

    public:
        virtual FactorablePointer createObject( const DocumentPtr & _doc );
        virtual FactorableVirtualInheritancePointer createVirtualInheritanceObject( const DocumentPtr & _doc );
        virtual void destroyObject( Factorable * _object );

    public:
        bool isEmptyObjects() const;
        uint32_t getCountObject() const;

    protected:
        virtual Factorable * _createObject() = 0;
        virtual void _destroyObject( Factorable * _object ) = 0;

    protected:
        ConstString m_type;

        ReferenceCounter m_count;

#ifdef MENGINE_DEBUG
        bool m_register;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Factory> FactoryPtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const ConstString & Factory::getType() const
    {
        return m_type;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class Type, class ... Args>
        FactoryPtr makeFactory( const DocumentPtr & _doc, Args && ... _args )
        {
            FactoryPtr factory = Helper::makeFactorableUnique<Type>( _doc, std::forward<Args>( _args ) ... );

            MENGINE_ASSERTION_MEMORY_PANIC( factory );

            const ConstString & type = Type::getFactorableType();

            factory->initialize( type );

            return factory;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
