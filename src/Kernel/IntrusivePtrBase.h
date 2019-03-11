#pragma once

#include "Config/Config.h"

#include "Config/Typedef.h"

#include "stdex/intrusive_ptr_base.h"

namespace Mengine
{
    class IntrusivePtrBase
    {
    public:
        IntrusivePtrBase() noexcept
        {
        }

        virtual ~IntrusivePtrBase()
        {
        }

    public:
        virtual void incref() = 0;
        virtual void decref() = 0;
        virtual uint32_t getrefcount() const = 0;

    public:
        inline static void intrusive_ptr_add_ref( IntrusivePtrBase * _ptr ) noexcept;
        inline static void intrusive_ptr_dec_ref( IntrusivePtrBase * _ptr ) noexcept;
        inline static uint32_t intrusive_ptr_get_ref( const IntrusivePtrBase * _ptr ) noexcept;
        
        template<class T>
        inline static void intrusive_ptr_setup( T *& _ptr, T * _other );

        template<class T>
        inline static void intrusive_ptr_release( T *& _ptr );

#	ifdef STDEX_INTRUSIVE_PTR_DEBUG
    public:
        inline static bool intrusive_ptr_check_ref( const IntrusivePtrBase * _ptr ) noexcept;
#	endif
    };
    //////////////////////////////////////////////////////////////////////////
    inline void IntrusivePtrBase::intrusive_ptr_add_ref( IntrusivePtrBase * _ptr ) noexcept
    {
        _ptr->incref();
    }
    //////////////////////////////////////////////////////////////////////////
    inline void IntrusivePtrBase::intrusive_ptr_dec_ref( IntrusivePtrBase * _ptr ) noexcept
    {
        _ptr->decref();
    }
    //////////////////////////////////////////////////////////////////////////
    inline uint32_t IntrusivePtrBase::intrusive_ptr_get_ref( const IntrusivePtrBase * _ptr ) noexcept
    {
        uint32_t refcount = _ptr->getrefcount();

        return refcount;
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    inline void IntrusivePtrBase::intrusive_ptr_setup( T *& _ptr, T * _other )
    {
        _ptr = _other;

        if( _other != nullptr )
        {
            IntrusivePtrBase::intrusive_ptr_add_ref( _other );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    inline void IntrusivePtrBase::intrusive_ptr_release( T *& _ptr )
    {
        if( _ptr != nullptr )
        {
            IntrusivePtrBase::intrusive_ptr_dec_ref( _ptr );
            _ptr = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
#	ifdef STDEX_INTRUSIVE_PTR_DEBUG
    //////////////////////////////////////////////////////////////////////////
    inline bool IntrusivePtrBase::intrusive_ptr_check_ref( const IntrusivePtrBase * _ptr ) noexcept
    {
        uint32_t refcount = _ptr->getrefcount();

        if( refcount == 0 )
        {
            return false;
        }

        return true;
    }
#	endif
}