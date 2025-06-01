#pragma once

#include "Kernel/IntrusivePtr.h"
#include "Kernel/IntrusivePtrBase.h"
#include "Kernel/IntrusiveSlug.h"
#include "Kernel/IntrusiveSlugLinkedPtr.h"

namespace Mengine
{
    template<class T>
    class IntrusiveSlugIterator
    {
    public:
        typedef T list_type;
        typedef typename list_type::value_type value_type;
        typedef typename list_type::derived_type derived_type;
        typedef IntrusivePtr<value_type, derived_type> value_type_ptr;
        typedef IntrusiveSlugLinkedPtr<value_type, derived_type, IntrusivePtr, IntrusivePtrBase> linked_type;
        typedef IntrusivePtr<linked_type, void> linked_type_ptr;
        typedef IntrusiveSlug<list_type> slug_type;

    public:
        explicit IntrusiveSlugIterator( const list_type & _list )
            : m_list( _list )
        {
            m_list.increfSlug( linked_type_ptr( &m_slug ) );
        }

        ~IntrusiveSlugIterator()
        {
            m_slug.unlink();
            m_list.decrefSlug();
        }

    public:
        void next_shuffle()
        {
            const linked_type * pos_right = m_slug.right_get();

            const linked_type * head = m_list.head();

            const linked_type * pos_right_adapt_right_adapt = nullptr;

            uint32_t countSlugs = m_list.countSlugs();

            if( countSlugs == 1 )
            {
                if( pos_right == head )
                {
                    return;
                }

                pos_right_adapt_right_adapt = pos_right->right_get();
            }
            else
            {
                const linked_type * pos_right_adapt = this->adapt_( pos_right );

                if( pos_right_adapt == head )
                {
                    return;
                }

                const linked_type * pos_right_adapt_right = pos_right_adapt->right_get();

                pos_right_adapt_right_adapt = this->adapt_( pos_right_adapt_right );
            }

            m_slug.unlink();

            pos_right_adapt_right_adapt->link_before( linked_type_ptr( &m_slug ) );
        }

        bool eof() const
        {
            const linked_type * pos = this->adapt_right();

            const linked_type * head = m_list.head();

            return pos == head;
        }

        const value_type * operator -> () const
        {
            const linked_type * linked = this->current();

            return static_cast<const value_type *>(linked);
        }

        const value_type * operator * () const
        {
            const linked_type * linked = this->current();

            return static_cast<const value_type *>(linked);
        }

        value_type * operator -> ()
        {
            linked_type * linked = this->current();

            return static_cast<value_type *>(linked);
        }

        value_type * operator * ()
        {
            linked_type * linked = this->current();

            return static_cast<value_type *>(linked);
        }

    protected:
        linked_type * current()
        {
            linked_type * pos = this->adapt_right();

            return pos;
        }

        const linked_type * current() const
        {
            const linked_type * pos = this->adapt_right();

            return pos;
        }

        linked_type * adapt_( linked_type * _carriage )
        {
            linked_type * carriage = _carriage;

            while( carriage->getIntrusiveTag() == stdex::EILT_SLUG )
            {
                carriage = carriage->right_get();
            }

            return carriage;
        }

        const linked_type * adapt_( const linked_type * _carriage ) const
        {
            const linked_type * carriage = _carriage;

            while( carriage->getIntrusiveTag() == stdex::EILT_SLUG )
            {
                carriage = carriage->right_get();
            }

            return carriage;
        }

        linked_type * adapt_right()
        {
            linked_type * carriage = m_slug.right_get();

            linked_type * right = adapt_( carriage );

            return right;
        }

        const linked_type * adapt_right() const
        {
            const linked_type * carriage = m_slug.right_get();

            const linked_type * right = adapt_( carriage );

            return right;
        }

    protected:
        const list_type & m_list;

        slug_type m_slug;
    };
}
