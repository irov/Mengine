#pragma once

#include "Config/Typedef.h"

#include "Interface/ServiceInterface.h"
#include "Interface/FactoryInterface.h"

#include "Kernel/FactorablePointer.h"
#include "Kernel/FactorableUnique.h"

#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Document.h"
#include "Kernel/Pointer.h"

#include "Kernel/ReferenceCounter.h"

#if defined(MENGINE_DEBUG)
#   include "Kernel/IntrusiveList.h"
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Factory
        : public FactoryInterface
        , public Factorable
    {
    public:
        Factory();
        ~Factory() override;

    public:
        bool initialize( const ConstString & _type );
        void finalize();

    public:
        MENGINE_INLINE const ConstString & getType() const override;

    public:
        FactorablePointer createObject( const DocumentPtr & _doc ) override;
        void destroyObject( Factorable * _object ) override;

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
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Factory, FactoryInterface> FactoryPtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const ConstString & Factory::getType() const
    {
        return m_type;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class Type, class ... Args>
        FactoryInterfacePtr makeFactory( const DocumentPtr & _doc, Args && ... _args )
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
