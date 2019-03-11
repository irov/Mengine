#pragma once

#include "Kernel/IntrusiveSlug.h"

#include "stdex/intrusive_slug_linked_ptr.h"

namespace Mengine
{ 
    template<class T>
    class IntrusiveSlugIterator
    {
    public:
        typedef T list_type;
        typedef typename list_type::value_type value_type;
        typedef IntrusivePtr<value_type> value_type_ptr;
        typedef stdex::intrusive_slug_linked_ptr<value_type, IntrusivePtr, IntrusivePtrBase> linked_type;
        typedef IntrusivePtr<linked_type> linked_type_ptr;
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

        inline bool eof() const
        {
            const linked_type * pos = this->adapt_right();

            const linked_type * head = m_list.head();

            return pos == head;
        }

        inline const value_type * operator -> () const
        {
            const linked_type * linked = this->current();

            return static_cast<const value_type *>(linked);
        }

        inline const value_type * operator * () const
        {
            const linked_type * linked = this->current();

            return static_cast<const value_type *>(linked);
        }

        inline value_type * operator -> ()
        {
            linked_type * linked = this->current();

            return static_cast<value_type *>(linked);
        }

        inline value_type * operator * ()
        {
            linked_type * linked = this->current();

            return static_cast<value_type *>(linked);
        }

    protected:
        inline linked_type * current()
        {
            linked_type * pos = this->adapt_right();

            return pos;
        }

        inline const linked_type * current() const
        {
            const linked_type * pos = this->adapt_right();

            return pos;
        }

        inline const linked_type * adapt_( const linked_type * _pos ) const
        {
            const linked_type * pos = _pos;

            while( pos->getIntrusiveTag() == stdex::EILT_SLUG )
            {
                pos = pos->right_get();
            }

            return pos;
        }

        inline linked_type * adapt_right()
        {
            linked_type * pos = m_slug.right_get();

            while( pos->getIntrusiveTag() == stdex::EILT_SLUG )
            {
                pos = pos->right_get();
            }

            return pos;
        }

        inline const linked_type * adapt_right() const
        {
            const linked_type * pos = m_slug.right_get();

            while( pos->getIntrusiveTag() == stdex::EILT_SLUG )
            {
                pos = pos->right_get();
            }

            return pos;
        }

    protected:
        const list_type & m_list;

        slug_type m_slug;
    };
}