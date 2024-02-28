#pragma once

#include "Kernel/Factory.h"

#include "Interface/ThreadMutexInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FactoryWithMutex
        : public FactoryInterface
    {
    public:
        FactoryWithMutex();
        ~FactoryWithMutex() override;

    public:
        bool initialize( const ConstString & _type );
        void finalize();

    public:
        MENGINE_INLINE const ConstString & getType() const override;

    public:
        FactorablePointer createObject( const DocumentInterfacePtr & _doc ) override;
        void destroyObject( Factorable * _object ) override;

    public:
        bool isEmptyObjects() const override;
        uint32_t getCountObject() const override;

    public:
        void setMutex( const ThreadMutexInterfacePtr & _mutex ) override;
        const ThreadMutexInterfacePtr & getMutex() const override;

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

        ThreadMutexInterfacePtr m_mutex;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FactoryWithMutex, FactoryInterface> FactoryWithMutexPtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const ConstString & FactoryWithMutex::getType() const
    {
        return m_type;
    }
    //////////////////////////////////////////////////////////////////////////
}
