#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/ThreadGuard.h"

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_FACTORABLE_DEBUG
#   ifdef MENGINE_DEBUG
#       define MENGINE_FACTORABLE_DEBUG 1
#   endif
#else
#   define MENGINE_FACTORABLE_DEBUG 0
#endif
//////////////////////////////////////////////////////////////////////////
#if MENGINE_DOCUMENT_ENABLE
#   include "Kernel/Documentable.h"
#endif
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Factory;
    //////////////////////////////////////////////////////////////////////////
    class Factorable
#if MENGINE_DOCUMENT_ENABLE
        : public Documentable
#else
        : public Mixin
#endif
    {
    public:
        Factorable();
        ~Factorable() override;

    public:
        void setFactory( Factory * _factory );
        MENGINE_INLINE Factory * getFactory() const noexcept;

    public:
        void setImmortal( bool _value );
        bool getImmortal() const;

    protected:
        uint32_t incref() override;
        void decref() override;
        uint32_t getrefcount() const override;

    private:
        virtual void destroy();

    protected:
        virtual void _destroy();

#if MENGINE_FACTORABLE_DEBUG
    public:
        bool isDestroyed() const;

    protected:
        virtual void _checkDestroy();
#endif

    public:
        MENGINE_INLINE uint32_t getReference() const noexcept;

#ifdef STDEX_INTRUSIVE_PTR_DEBUG
    public:
        MENGINE_INLINE static bool intrusive_ptr_check_ref( const Factorable * _ptr );
#endif

    protected:
        uint32_t m_reference;

        Factory * m_factory;

#if MENGINE_FACTORABLE_DEBUG
        bool m_destroy;
        bool m_immortal;
#endif

        MENGINE_THREAD_GUARD_INIT( Factorable );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Factorable> FactorablePtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE Factory * Factorable::getFactory() const noexcept
    {
        return m_factory;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE uint32_t Factorable::getReference() const noexcept
    {
        return m_reference;
    }
    //////////////////////////////////////////////////////////////////////////
#ifdef STDEX_INTRUSIVE_PTR_DEBUG
    MENGINE_INLINE bool Factorable::intrusive_ptr_check_ref( const Factorable * _ptr )
    {
        if( _ptr == nullptr )
        {
            return false;
        }

#if MENGINE_FACTORABLE_DEBUG
        if( _ptr->isDestroyed() == true )
        {
            return false;
        }
#endif

        if( _ptr->getReference() == 0 )
        {
            return false;
        }

        return true;
    }
#endif
    //////////////////////////////////////////////////////////////////////////
}
