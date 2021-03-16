#pragma once

#include "Config/Typedef.h"

#include "Interface/ServiceInterface.h"

#include "Kernel/FactorablePointer.h"
#include "Kernel/FactorableUnique.h"

#include "Kernel/Assertion.h"
#include "Kernel/Document.h"
#include "Kernel/Pointer.h"

#include "Kernel/ThreadGuard.h"

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
        virtual void destroyObject( Factorable * _object );

    public:
        bool isEmptyObjects() const;
        uint32_t getCountObject() const;

    protected:
        virtual Factorable * _createObject() = 0;
        virtual void _destroyObject( Factorable * _object ) = 0;

    protected:
        ConstString m_type;

        uint32_t m_count;

#ifdef MENGINE_DEBUG        
        bool m_register;
#endif

        MENGINE_THREAD_GUARD_INIT( Factory );
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
        template<class T, class ... Args>
        FactoryPtr makeFactory( const DocumentPtr & _doc, Args && ... _args )
        {
            FactoryPtr factory = Helper::makeFactorableUnique<T>( _doc, std::forward<Args>( _args ) ... );

            return factory;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
