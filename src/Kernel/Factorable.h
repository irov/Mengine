#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/ConstString.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/ReferenceCounter.h"

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
        void setFactory( FactoryInterface * _factory );
        MENGINE_INLINE FactoryInterface * getFactory() const MENGINE_NOEXCEPT;

    public:
        const ConstString & getType() const;

    public:
        void setImmortal( bool _value );
        bool getImmortal() const;

    protected:
        uint32_t incref();
        void decref();

    public:
        uint32_t getrefcount() const;

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

    protected:
        ReferenceCounter m_reference;

        FactoryInterface * m_factory;

#if defined(MENGINE_FACTORABLE_DEBUG_ENABLE)
        bool m_destroy;
        bool m_immortal;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Factorable> FactorablePtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE FactoryInterface * Factorable::getFactory() const MENGINE_NOEXCEPT
    {
        return m_factory;
    }
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
#define DECLARE_FACTORABLE_NONE(Type)\
    public:\
    static const Mengine::ConstString & getFactorableType(){return Mengine::ConstString::none();}\
    protected:
//////////////////////////////////////////////////////////////////////////

