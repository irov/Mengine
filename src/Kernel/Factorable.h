#pragma once

#include "Kernel/Mixin.h"

#ifndef MENGINE_FACTORABLE_DEBUG_ENABLE
#   ifdef MENGINE_DEBUG
#       define MENGINE_FACTORABLE_DEBUG
#   endif
#else
#   define MENGINE_FACTORABLE_DEBUG
#endif

//typedef IntrusivePtr<Type> TypePtr;

namespace Mengine
{
    class Factory;

    class Factorable
        : public Mixin
    {
    public:
        Factorable();
        ~Factorable() override;

    public:
        void setFactory( Factory * _factory );
        MENGINE_INLINE Factory * getFactory() const noexcept;

    public:
        void setImmortal( bool _value );

    protected:
        void incref() override;
        void decref() override;
        uint32_t getrefcount() const override;

    private:
        virtual void destroy();

    protected:
        virtual void _destroy();

#ifdef MENGINE_FACTORABLE_DEBUG
    public:
        bool isDestroyed() const;

    protected:
        virtual void _checkDestroy();
#endif

    public:
        uint32_t getReference() const;

#ifdef STDEX_INTRUSIVE_PTR_DEBUG
    public:
        MENGINE_INLINE static bool intrusive_ptr_check_ref( const Factorable * _ptr );
#endif

    protected:
        uint32_t m_reference;

        Factory * m_factory;

#ifdef MENGINE_FACTORABLE_DEBUG
        bool m_destroy;
        bool m_immortal;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Factorable> FactorablePtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE Factory * Factorable::getFactory() const noexcept
    {
        return m_factory;
    }
    //////////////////////////////////////////////////////////////////////////
#ifdef STDEX_INTRUSIVE_PTR_DEBUG
    MENGINE_INLINE bool Factorable::intrusive_ptr_check_ref( const Factorable * _ptr )
    {
        if( _ptr == nullptr )
        {
            return false;
        }

#ifdef MENGINE_FACTORABLE_DEBUG
        if( _ptr->isDestroyed() == true )
        {
            return false;
        }
#endif

        if( _ptr->m_reference == 0 )
        {
            return false;
        }

        return true;
    }
#endif
}
