#	pragma once

#   include "Config/Typedef.h"

#	include "Core/IntrusivePtr.h"
#	include "Core/IntrusiveLinked.h"
#	include "Factory/Factorable.h"

namespace Menge
{
    class ConstStringHolder
        : public IntrusiveLinked<ConstStringHolder>
        , public Factorable
    {
    protected:
        ConstStringHolder();        

    private:
        ConstStringHolder & operator = ( const ConstStringHolder & _holder );

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
        virtual const char * _c_str() const
        {
            return "";
        }

    protected:
        virtual void releaseString() 
        {
            //Empty 
        }

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
        friend inline void intrusive_ptr_add_ref( ConstStringHolder * _ptr )
        {
            ++_ptr->m_reference;
            ++_ptr->m_owner->m_reference;
        }

        friend inline void intrusive_ptr_release( ConstStringHolder * _ptr )
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

    protected:
        void setup( const char * _data, size_t _size );

    protected:
        size_t m_reference;

        size_t m_hash;
        size_t m_size;

        mutable ConstStringHolder * m_owner;
    };

    typedef IntrusivePtr<ConstStringHolder> ConstStringHolderPtr;
}