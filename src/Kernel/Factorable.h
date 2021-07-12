#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/ConstString.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/ReferenceCounter.h"

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
        const ConstString & getType() const;

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
#endif

    public:
        MENGINE_INLINE uint32_t getReference() const noexcept;

#ifdef STDEX_INTRUSIVE_PTR_DEBUG
    public:
        MENGINE_INLINE static bool intrusive_ptr_check_ref( const Factorable * _ptr );
#endif

    protected:
        ReferenceCounter m_reference;

        Factory * m_factory;

#if MENGINE_FACTORABLE_DEBUG
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
    MENGINE_INLINE uint32_t Factorable::getReference() const noexcept
    {
        uint32_t count = m_reference.getReferenceCount();

        return count;
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
//////////////////////////////////////////////////////////////////////////
#define DECLARE_FACTORABLE(Type)\
    public:\
    static const Mengine::ConstString & getFactorableType(){return STRINGIZE_STRING_LOCAL( #Type );}\
    protected:
//////////////////////////////////////////////////////////////////////////
#define DECLARE_FACTORABLE_NONE(Type)\
    public:\
    static const Mengine::ConstString & getFactorableType(){return Mengine::ConstString::none();}\
    protected:
//////////////////////////////////////////////////////////////////////////

