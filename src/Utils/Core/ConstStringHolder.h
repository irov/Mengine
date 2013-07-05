#	pragma once

#	include "Core/IntrusivePtr.h"
#	include "Core/IntrusiveLinked.h"

#	include "Factory/Factorable.h"

#   include <stddef.h>

namespace Menge
{
    class ConstStringHolder
        : public IntrusiveLinked<ConstStringHolder>
        , public Factorable
    {
    protected:
        ConstStringHolder()
            : m_reference(0)
            , m_data(nullptr)
            , m_hash(0)
            , m_size(0)
        {
            m_owner = this;
        }

    private:
        ConstStringHolder & operator = ( const ConstStringHolder & _holder )
        {
            (void)_holder;

            return *this;
        }

    public:
        inline const char * c_str() const
        {
            return m_owner->_c_str();
        }

        inline size_t size() const
        {
            return m_size;
        }

        inline bool empty() const
        {
            return m_size == 0;
        }

    protected:
        const char * _c_str() const
        {
            return m_data;
        }

    protected:
        void releaseString()
        {
            m_data = nullptr;

            this->_releaseString();
        }

        virtual void _releaseString() = 0;

    public:
        inline ConstStringHolder * owner() const
        {
            return m_owner;
        }

        inline size_t hash() const
        {
            return m_hash;
        }

    public:
        bool less( ConstStringHolder * _holder );
        bool equal( ConstStringHolder * _holder );

    public:
        void combine( ConstStringHolder * _holder );

    protected:        
        class ForeachCombineOwner;      
        class ForeachCombineOther;

        void combine_owner( ConstStringHolder * _out );
        void combine_other( ConstStringHolder * _out );
        void combine_from( ConstStringHolder * _from, ConstStringHolder * _out );

    protected:
        friend inline void intrusive_ptr_add_ref( ConstStringHolder * _ptr );
        friend inline void intrusive_ptr_release( ConstStringHolder * _ptr );

    protected:
        void setup( const char * _data, size_t _size );

    protected:
        size_t m_reference;

        const char * m_data;

        size_t m_hash;
        size_t m_size;

        mutable ConstStringHolder * m_owner;
    };
    //////////////////////////////////////////////////////////////////////////
    inline void intrusive_ptr_add_ref( ConstStringHolder * _ptr )
    {
        ++_ptr->m_reference;
        ++_ptr->m_owner->m_reference;
    }
    //////////////////////////////////////////////////////////////////////////
    inline void intrusive_ptr_release( ConstStringHolder * _ptr )
    {
        if( --_ptr->m_owner->m_reference == 0 )
        {
            _ptr->m_owner->destroy();
        }

        if( --_ptr->m_reference == 0 )
        {
            _ptr->destroy();
        }
    }
}