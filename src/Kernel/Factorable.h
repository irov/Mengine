#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/ConstString.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/ReferenceCounter.h"

#include "Config/UniqueId.h"

#if defined(MENGINE_DEBUG)
#include "Config/Timestamp.h"
#endif

//////////////////////////////////////////////////////////////////////////
#if !defined(MENGINE_FACTORABLE_DEBUG)
#   if defined(MENGINE_DEBUG)
#       define MENGINE_FACTORABLE_DEBUG 1
#   else
#       define MENGINE_FACTORABLE_DEBUG 0
#   endif
#endif
//////////////////////////////////////////////////////////////////////////
#if MENGINE_FACTORABLE_DEBUG == 1
#   define MENGINE_FACTORABLE_DEBUG_ENABLE
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_DOCUMENT_ENABLE)
#   include "Kernel/Documentable.h"
#endif
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FactoryInterface;
    //////////////////////////////////////////////////////////////////////////
    class Factorable
#if defined(MENGINE_DOCUMENT_ENABLE)
        : public Documentable
#else
        : public Mixin
#endif
#if defined(MENGINE_DEBUG)
        , public IntrusiveLinked<Factorable>
#endif
    {
    public:
        Factorable();
        ~Factorable() override;

    public:
        void setFactory( FactoryInterface * _factorableFactory );
        MENGINE_INLINE FactoryInterface * getFactory() const;

    public:
        void setUniqueIdentity( UniqueId _factorableId );
        MENGINE_INLINE UniqueId getUniqueIdentity() const;

#if defined(MENGINE_FACTORABLE_DEBUG_ENABLE)
    public:
        void setFactorableTimestamp( Timestamp _factorableTimestamp );
        MENGINE_INLINE Timestamp getFactorableTimestamp() const;
#endif

    public:
        const ConstString & getType() const;

    protected:
        uint32_t incref() override;
        void decref() override;

    public:
        uint32_t getrefcount() const override;

    private:
        virtual void destroy();

    protected:
        virtual void _destroy();

#if defined(MENGINE_FACTORABLE_DEBUG_ENABLE)
    public:
        bool isDestroyed() const;
#endif

#if defined(STDEX_INTRUSIVE_PTR_DEBUG)
    public:
        MENGINE_INLINE static bool intrusive_ptr_check_ref( const Factorable * _ptr );
#endif

    private:
        FactoryInterface * m_factorableFactory;

        ReferenceCounter m_factorableReference;

        UniqueId m_factorableId;

#if defined(MENGINE_FACTORABLE_DEBUG_ENABLE)
        Timestamp m_factorableTimestamp;

        bool m_factorableDestroy;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Factorable> FactorablePtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE FactoryInterface * Factorable::getFactory() const
    {
        return m_factorableFactory;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE UniqueId Factorable::getUniqueIdentity() const
    {
        return m_factorableId;
    }
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_FACTORABLE_DEBUG_ENABLE)
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE Timestamp Factorable::getFactorableTimestamp() const
    {
        return m_factorableTimestamp;
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
#if defined(STDEX_INTRUSIVE_PTR_DEBUG)
    MENGINE_INLINE bool Factorable::intrusive_ptr_check_ref( const Factorable * _ptr )
    {
        if( _ptr == nullptr )
        {
            return false;
        }

#if defined(MENGINE_FACTORABLE_DEBUG_ENABLE)
        if( _ptr->isDestroyed() == true )
        {
            return false;
        }
#endif

        if( _ptr->getrefcount() == 0 )
        {
            return false;
        }

        return true;
    }
#endif
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define DECLARE_FACTORABLE(Type)\
    public:\
    static const Mengine::ConstString & getFactorableType(){return STRINGIZE_STRING_LOCAL( #Type );}\
    protected:
//////////////////////////////////////////////////////////////////////////

