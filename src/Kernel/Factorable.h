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
#if MENGINE_FACTORABLE_DEBUG == 1
#   define MENGINE_FACTORABLE_DEBUG_ENABLE
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_DOCUMENT_ENABLE
#   include "Kernel/Documentable.h"
#endif
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FactoryInterface;
    //////////////////////////////////////////////////////////////////////////
    class Factorable
#ifdef MENGINE_DOCUMENT_ENABLE
        : public Documentable
#else
        : public Mixin
#endif
#ifdef MENGINE_DEBUG
        , public IntrusiveLinked<Factorable>
#endif
    {
    public:
        Factorable();
        ~Factorable() override;

    public:
        void setFactory( FactoryInterface * _factory );
        MENGINE_INLINE FactoryInterface * getFactory() const noexcept;

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

#ifdef MENGINE_FACTORABLE_DEBUG_ENABLE
    public:
        bool isDestroyed() const;
#endif

#ifdef STDEX_INTRUSIVE_PTR_DEBUG
    public:
        MENGINE_INLINE static bool intrusive_ptr_check_ref( const Factorable * _ptr );
#endif

    protected:
        ReferenceCounter m_reference;

        FactoryInterface * m_factory;

#ifdef MENGINE_FACTORABLE_DEBUG_ENABLE
        bool m_destroy;
        bool m_immortal;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Factorable> FactorablePtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE FactoryInterface * Factorable::getFactory() const noexcept
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

#ifdef MENGINE_FACTORABLE_DEBUG_ENABLE
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
#define DECLARE_FACTORABLE_NONE(Type)\
    public:\
    static const Mengine::ConstString & getFactorableType(){return Mengine::ConstString::none();}\
    protected:
//////////////////////////////////////////////////////////////////////////

