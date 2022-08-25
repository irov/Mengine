#pragma once

#include "Kernel/Hashgen.h"
#include "Kernel/IntrusivePtr.h"
#include "Kernel/MemoryAllocator.h"
#include "Kernel/Assertion.h"

#ifndef MENGINE_HASHTABLE_DUMMY_ELEMENT
#define MENGINE_HASHTABLE_DUMMY_ELEMENT (reinterpret_cast<element_type *>(MENGINE_MAX_ALIGNED_POINTER_VALUE))
#endif

#ifndef MENGINE_HASHTABLE_PERTURB_SHIFT
#define MENGINE_HASHTABLE_PERTURB_SHIFT 5
#endif

namespace Mengine
{
    template<class K, class T, class H = Hashgen<K>>
    class Hashtable
    {
    public:
        typedef uint32_t size_type;
        typedef HashType hash_type;
        typedef K key_type;
        typedef T element_type_ptr;
        typedef H hashgen_type;
        typedef typename element_type_ptr::value_type element_type;

        struct value_type
        {
            hash_type hash = 0;
            key_type key;
            element_type_ptr element;
        };

    public:
        Hashtable()
            : m_size( 0 )
            , m_dummy( 0 )
            , m_capacity( 0 )
            , m_buffer( nullptr )
        {
        }

        ~Hashtable()
        {
            this->clear();

            Helper::deallocateMemory( m_buffer, "hashtable" );
        }

    private:
        Hashtable( const Hashtable & ) = delete;
        Hashtable( Hashtable && ) = delete;

    public:
        const element_type_ptr & emplace( const key_type & _key, const element_type_ptr & _element )
        {
            MENGINE_ASSERTION_FATAL( _element != nullptr );

            size_type test_size = (m_size + m_dummy) * 3 + 1;
            size_type test_capacity = m_capacity * 2;

            if( test_size > test_capacity )
            {
                this->increase_();
            }

            hash_type hash = hashgen_type()(_key);

            const element_type_ptr & element = this->push_( hash, _key, _element );

            return element;
        }

        element_type_ptr change( const key_type & _key, const element_type_ptr & _element )
        {
            MENGINE_ASSERTION_FATAL( _element != nullptr );

            size_type test_size = (m_size + m_dummy) * 3 + 1;
            size_type test_capacity = m_capacity * 2;

            if( test_size > test_capacity )
            {
                this->increase_();
            }

            hash_type hash = hashgen_type()(_key);

            element_type_ptr prev = this->change_( hash, _key, _element );

            return prev;
        }

        element_type_ptr erase( const key_type & _key )
        {
            if( m_size == 0 )
            {
                return nullptr;
            }

            hash_type hash = hashgen_type()(_key);

            element_type_ptr element = this->pop_( hash, _key );

            return element;
        }

        element_type_ptr erase( const value_type & _value )
        {
            if( m_size == 0 )
            {
                return nullptr;
            }

            element_type_ptr element = this->pop_( _value.hash, _value.key );

            return element;
        }

        const element_type_ptr & find( const key_type & _key ) const
        {
            if( m_size == 0 )
            {
                return element_type_ptr::none();
            }

            hash_type hash = hashgen_type()(_key);

            const element_type_ptr & element = this->find_( hash, _key );

            return element;
        }

        bool exist( const key_type & _key ) const
        {
            if( m_size == 0 )
            {
                return false;
            }

            hash_type hash = hashgen_type()(_key);

            bool result = this->exist_( hash, _key );

            return result;
        }

    public:
        void reserve( uint32_t _capacity )
        {
            if( m_capacity > _capacity )
            {
                return;
            }

            --_capacity;
            _capacity |= _capacity >> 16;
            _capacity |= _capacity >> 8;
            _capacity |= _capacity >> 4;
            _capacity |= _capacity >> 2;
            _capacity |= _capacity >> 1;
            ++_capacity;

            this->rebalance_( _capacity );
        }

        bool empty() const
        {
            return m_size == 0;
        }

        uint32_t size() const
        {
            return m_size;
        }

        void clear()
        {
            for( size_type index = 0; index != m_capacity; ++index )
            {
                value_type * record = m_buffer + index;

                if( record->element.get_base() == nullptr )
                {
                    continue;
                }

                if( record->element.get_base() == MENGINE_HASHTABLE_DUMMY_ELEMENT )
                {
                    record->element.set( nullptr );
                    record->key = key_type();

                    continue;
                }

                record->element = nullptr;
                record->key = key_type();
            }

            m_size = 0;
            m_dummy = 0;
        }

    public:
        class const_iterator
        {
        public:
            const_iterator( value_type * _carriage, value_type * _end )
                : m_carriage( _carriage )
                , m_end( _end )
            {
                this->adapt_();
            }

            const_iterator( const const_iterator & _it )
                : m_carriage( _it.m_carriage )
                , m_end( _it.m_end )
            {
            }

        public:
            const_iterator & operator = ( const const_iterator & _it )
            {
                this->m_carriage = _it.m_carriage;
                this->m_end = _it.m_end;

                return *this;
            }

        public:
            const value_type * operator -> () const
            {
                return m_carriage;
            }

            const value_type & operator * () const
            {
                return *m_carriage;
            }

        public:
            bool operator == ( const const_iterator & _it ) const
            {
                return m_carriage == _it.m_carriage;
            }

            bool operator != ( const const_iterator & _it ) const
            {
                return !this->operator == ( _it );
            }

            const_iterator & operator ++ ()
            {
                ++m_carriage;

                this->adapt_();

                return *this;
            }

            const_iterator operator ++ ( int )
            {
                const_iterator tmp = *this;
                ++ * this;

                return tmp;
            }

        protected:
            void adapt_()
            {
                for( ; m_carriage != m_end; ++m_carriage )
                {
                    if( m_carriage->element.get_base() == nullptr || m_carriage->element.get_base() == MENGINE_HASHTABLE_DUMMY_ELEMENT )
                    {
                        continue;
                    }

                    break;
                }
            }

        protected:
            value_type * m_carriage;
            value_type * m_end;
        };

        class const_reverse_iterator
        {
        public:
            const_reverse_iterator( value_type * _carriage, value_type * _end )
                : m_carriage( _carriage )
                , m_end( _end )
            {
                this->adapt_();
            }

            const_reverse_iterator( const const_reverse_iterator & _it )
                : m_carriage( _it.m_carriage )
                , m_end( _it.m_end )
            {
            }

        public:
            const_reverse_iterator & operator = ( const const_reverse_iterator & _it )
            {
                this->m_carriage = _it.m_carriage;
                this->m_end = _it.m_end;

                return *this;
            }

        public:
            const value_type * operator -> () const
            {
                return (m_carriage - 1);
            }

            const value_type & operator * () const
            {
                return *(m_carriage - 1);
            }

        public:
            bool operator == ( const const_reverse_iterator & _it ) const
            {
                return m_carriage == _it.m_carriage;
            }

            bool operator != ( const const_reverse_iterator & _it ) const
            {
                return !this->operator == ( _it );
            }

            const_reverse_iterator & operator ++ ()
            {
                --m_carriage;

                this->adapt_();

                return *this;
            }

            const_iterator operator ++ ( int )
            {
                const_iterator tmp = *this;
                -- * this;

                return tmp;
            }

        protected:
            void adapt_()
            {
                for( ; m_carriage != m_end; --m_carriage )
                {
                    const value_type * value = m_carriage - 1;

                    if( value->element.get_base() == nullptr || value->element.get_base() == MENGINE_HASHTABLE_DUMMY_ELEMENT )
                    {
                        continue;
                    }

                    break;
                }
            }

        protected:
            value_type * m_carriage;
            value_type * m_end;
        };

    public:
        const_iterator begin() const
        {
            return const_iterator( m_buffer, m_buffer + m_capacity );
        }

        const_iterator end() const
        {
            return const_iterator( m_buffer + m_capacity, m_buffer + m_capacity );
        }

        const_reverse_iterator rbegin() const
        {
            return const_reverse_iterator( m_buffer + m_capacity, m_buffer );
        }

        const_reverse_iterator rend() const
        {
            return const_reverse_iterator( m_buffer, m_buffer );
        }

    protected:
        void increase_()
        {
            if( m_buffer == nullptr )
            {
                m_capacity = 8;
                m_buffer = Hashtable::newbuffer_( m_capacity );

                return;
            }

            size_type new_capacity = (m_capacity << 1);

            this->rebalance_( new_capacity );
        }

    protected:
        static value_type * newbuffer_( size_type _capacity )
        {
            value_type * new_buffer = Helper::allocateMemoryNT<value_type>( _capacity, "hashtable" );

            for( size_type index = 0; index != _capacity; ++index )
            {
                value_type * record = new_buffer + index;

                new(record) value_type;
            }

            return new_buffer;
        }

        void rebalance_( size_type _capacity )
        {
            m_size = 0;
            m_dummy = 0;

            size_type old_capacity = m_capacity;
            value_type * old_buffer = m_buffer;

            value_type * new_buffer = Hashtable::newbuffer_( _capacity );

            m_capacity = _capacity;
            m_buffer = new_buffer;

            for( size_type index = 0; index != old_capacity; ++index )
            {
                value_type * record = old_buffer + index;

                if( record->element.get_base() == nullptr || record->element.get_base() == MENGINE_HASHTABLE_DUMMY_ELEMENT )
                {
                    continue;
                }

                this->push_( record->hash, record->key, record->element );

                record->key = key_type();
                record->element = nullptr;
            }

            Helper::deallocateMemory( old_buffer, "hashtable" );
        }

        const element_type_ptr & push_( hash_type _hash, const key_type & _key, const element_type_ptr & _element )
        {
            uint64_t hash_mask = m_capacity - 1;
            uint64_t mask = (uint64_t)_hash & hash_mask;

            for( uint64_t perturb = (uint64_t)_hash; ; perturb >>= MENGINE_HASHTABLE_PERTURB_SHIFT )
            {
                value_type * record = m_buffer + (mask & hash_mask);

                MENGINE_ASSERTION_FATAL( !(record->hash == _hash && record->key == _key && record->element.get_base() != nullptr && record->element.get_base() != MENGINE_HASHTABLE_DUMMY_ELEMENT), "hash push" );

                if( record->element.get_base() == nullptr )
                {
                    record->hash = _hash;
                    record->key = _key;
                    record->element = _element;

                    ++m_size;

                    return record->element;
                }

                if( record->element.get_base() == MENGINE_HASHTABLE_DUMMY_ELEMENT )
                {
                    record->hash = _hash;
                    record->key = _key;
                    record->element.set( nullptr );
                    record->element = _element;

                    MENGINE_ASSERTION_FATAL( m_dummy > 0 );

                    --m_dummy;
                    ++m_size;

                    return record->element;
                }

                mask = (mask << 2) + mask + perturb + 1;
            }
        }

        element_type_ptr change_( hash_type _hash, const key_type & _key, const element_type_ptr & _element )
        {
            uint64_t hash_mask = m_capacity - 1;
            uint64_t mask = (uint64_t)_hash & hash_mask;

            for( uint64_t perturb = (uint64_t)_hash; ; perturb >>= MENGINE_HASHTABLE_PERTURB_SHIFT )
            {
                value_type * record = m_buffer + (mask & hash_mask);

                if( record->hash == _hash && record->key == _key )
                {
                    if( record->element.get_base() == MENGINE_HASHTABLE_DUMMY_ELEMENT )
                    {
                        record->element.set( nullptr );
                        record->element = _element;

                        ++m_size;
                        --m_dummy;

                        return nullptr;
                    }

                    if( record->element.get_base() != nullptr )
                    {
                        element_type_ptr prev = std::move( record->element );
                        record->element = _element;

                        return prev;
                    }
                }

                if( record->element.get_base() == nullptr )
                {
                    record->hash = _hash;
                    record->key = _key;
                    record->element = _element;

                    ++m_size;

                    return nullptr;
                }

                mask = (mask << 2) + mask + perturb + 1;
            }
        }

        element_type_ptr pop_( hash_type _hash, const key_type & _key )
        {
            uint64_t hash_mask = m_capacity - 1;
            uint64_t mask = (uint64_t)_hash & hash_mask;

            for( uint64_t perturb = (uint64_t)_hash; ; perturb >>= MENGINE_HASHTABLE_PERTURB_SHIFT )
            {
                value_type * record = m_buffer + (mask & hash_mask);

                if( record->element.get_base() == nullptr )
                {
                    return nullptr;
                }

                if( record->hash == _hash && record->key == _key && record->element.get_base() != MENGINE_HASHTABLE_DUMMY_ELEMENT )
                {
                    element_type_ptr pop_element = std::move( record->element );

                    record->element.set( MENGINE_HASHTABLE_DUMMY_ELEMENT );

                    ++m_dummy;
                    --m_size;

                    return pop_element;
                }

                mask = (mask << 2) + mask + perturb + 1;
            }
        }

        const element_type_ptr & find_( hash_type _hash, const key_type & _key ) const
        {
            uint64_t hash_mask = m_capacity - 1;
            uint64_t mask = (uint64_t)_hash & hash_mask;

            for( uint64_t perturb = (uint64_t)_hash; ; perturb >>= MENGINE_HASHTABLE_PERTURB_SHIFT )
            {
                value_type * record = m_buffer + (mask & hash_mask);

                if( record->element.get_base() == nullptr )
                {
                    return element_type_ptr::none();
                }

                if( record->hash == _hash && record->key == _key )
                {
                    if( record->element.get_base() == MENGINE_HASHTABLE_DUMMY_ELEMENT )
                    {
                        return element_type_ptr::none();
                    }

                    return record->element;
                }

                mask = (mask << 2) + mask + perturb + 1;
            }
        }

        bool exist_( hash_type _hash, const key_type & _key ) const
        {
            uint64_t hash_mask = m_capacity - 1;
            uint64_t mask = (uint64_t)_hash & hash_mask;

            for( uint64_t perturb = (uint64_t)_hash; ; perturb >>= MENGINE_HASHTABLE_PERTURB_SHIFT )
            {
                value_type * record = m_buffer + (mask & hash_mask);

                if( record->element.get_base() == nullptr )
                {
                    return false;
                }

                if( record->hash == _hash && record->key == _key )
                {
                    if( record->element.get_base() == MENGINE_HASHTABLE_DUMMY_ELEMENT )
                    {
                        return false;
                    }

                    return true;
                }

                mask = (mask << 2) + mask + perturb + 1;
            }
        }

    protected:
        size_type m_size;
        size_type m_dummy;
        size_type m_capacity;

        value_type * m_buffer;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class K, class T, class H>
    class Hashtable<K, T *, H>
    {
    public:
        typedef uint32_t size_type;
        typedef HashType hash_type;
        typedef K key_type;
        typedef T * element_type_ptr;
        typedef H hashgen_type;
        typedef T element_type;

        struct value_type
        {
            hash_type hash = 0;
            key_type key;
            element_type_ptr element = nullptr;
        };

    public:
        Hashtable()
            : m_size( 0 )
            , m_dummy( 0 )
            , m_capacity( 0 )
            , m_buffer( nullptr )
        {
        }

        ~Hashtable()
        {
            this->clear();

            Helper::deallocateMemory( m_buffer, "hashtable" );
        }

    private:
        Hashtable( const Hashtable & ) = delete;
        Hashtable( Hashtable && ) = delete;

    public:
        element_type_ptr emplace( const key_type & _key, element_type_ptr _element )
        {
            MENGINE_ASSERTION_FATAL( _element != nullptr );

            uint32_t test_size = (m_size + m_dummy) * 3 + 1;
            uint32_t test_capacity = m_capacity * 2;
            if( test_size > test_capacity )
            {
                this->increase_();
            }

            hash_type hash = hashgen_type()(_key);

            this->push_( hash, _key, _element );

            return _element;
        }

        element_type_ptr change( const key_type & _key, element_type_ptr _element )
        {
            MENGINE_ASSERTION_FATAL( _element != nullptr );

            uint32_t test_size = (m_size + m_dummy) * 3 + 1;
            uint32_t test_capacity = m_capacity * 2;
            if( test_size > test_capacity )
            {
                this->increase_();
            }

            hash_type hash = hashgen_type()(_key);

            element_type_ptr prev = this->change_( hash, _key, _element );

            return prev;
        }

        element_type_ptr erase( const key_type & _key )
        {
            if( m_size == 0 )
            {
                return nullptr;
            }

            hash_type hash = hashgen_type()(_key);

            element_type_ptr element = this->pop_( hash, _key );

            return element;
        }

        element_type_ptr erase( const value_type & _value )
        {
            if( m_size == 0 )
            {
                return nullptr;
            }

            element_type_ptr element = this->pop_( _value.hash, _value.key );

            return element;
        }

        element_type_ptr find( const key_type & _key ) const
        {
            if( m_size == 0 )
            {
                return nullptr;
            }

            hash_type hash = hashgen_type()(_key);

            element_type_ptr element = this->find_( hash, _key );

            return element;
        }

        bool exist( const key_type & _key ) const
        {
            if( m_size == 0 )
            {
                return false;
            }

            hash_type hash = hashgen_type()(_key);

            bool result = this->exist_( hash, _key );

            return result;
        }

    public:
        void reserve( uint32_t _capacity )
        {
            if( m_capacity > _capacity )
            {
                return;
            }

            --_capacity;
            _capacity |= _capacity >> 16;
            _capacity |= _capacity >> 8;
            _capacity |= _capacity >> 4;
            _capacity |= _capacity >> 2;
            _capacity |= _capacity >> 1;
            ++_capacity;

            this->rebalance_( _capacity );
        }

        bool empty() const
        {
            return m_size == 0;
        }

        uint32_t size() const
        {
            return m_size;
        }

        void clear()
        {
            for( size_type index = 0; index != m_capacity; ++index )
            {
                value_type * record = m_buffer + index;

                if( record->element == nullptr )
                {
                    continue;
                }

                record->element = nullptr;
                record->key = key_type();
            }

            m_size = 0;
            m_dummy = 0;
        }

    public:
        class const_iterator
        {
        public:
            const_iterator( value_type * _carriage, value_type * _end )
                : m_carriage( _carriage )
                , m_end( _end )
            {
                this->adapt_();
            }

            const_iterator( const const_iterator & _it )
                : m_carriage( _it.m_carriage )
                , m_end( _it.m_end )
            {
            }

        public:
            const_iterator & operator = ( const const_iterator & _it )
            {
                this->m_carriage = _it.m_carriage;
                this->m_end = _it.m_end;

                return *this;
            }

        public:
            const value_type * operator -> () const
            {
                return m_carriage;
            }

            const value_type & operator * () const
            {
                return *m_carriage;
            }

        public:
            bool operator == ( const const_iterator & _it ) const
            {
                return m_carriage == _it.m_carriage;
            }

            bool operator != ( const const_iterator & _it ) const
            {
                return !this->operator == ( _it );
            }

            const_iterator & operator ++ ()
            {
                ++m_carriage;

                this->adapt_();

                return *this;
            }

            const_iterator operator ++ ( int )
            {
                const_iterator tmp = *this;
                ++ * this;

                return tmp;
            }

        protected:
            void adapt_()
            {
                for( ; m_carriage != m_end; ++m_carriage )
                {
                    const value_type * value = m_carriage;

                    if( value->element == nullptr || value->element == MENGINE_HASHTABLE_DUMMY_ELEMENT )
                    {
                        continue;
                    }

                    break;
                }
            }

        protected:
            value_type * m_carriage;
            value_type * m_end;
        };

        class const_reverse_iterator
        {
        public:
            const_reverse_iterator( value_type * _carriage, value_type * _end )
                : m_carriage( _carriage )
                , m_end( _end )
            {
                this->adapt_();
            }

            const_reverse_iterator( const const_reverse_iterator & _it )
                : m_carriage( _it.m_carriage )
                , m_end( _it.m_end )
            {
            }

        public:
            const_reverse_iterator & operator = ( const const_reverse_iterator & _it )
            {
                this->m_carriage = _it.m_carriage;
                this->m_end = _it.m_end;

                return *this;
            }

        public:
            const value_type * operator -> () const
            {
                return (m_carriage - 1);
            }

            const value_type & operator * () const
            {
                return *(m_carriage - 1);
            }

        public:
            bool operator == ( const const_reverse_iterator & _it ) const
            {
                return m_carriage == _it.m_carriage;
            }

            bool operator != ( const const_reverse_iterator & _it ) const
            {
                return !this->operator == ( _it );
            }

            const_reverse_iterator & operator ++ ()
            {
                --m_carriage;

                this->adapt_();

                return *this;
            }

            const_iterator operator ++ ( int )
            {
                const_iterator tmp = *this;
                -- * this;

                return tmp;
            }

        protected:
            void adapt_()
            {
                for( ; m_carriage != m_end; --m_carriage )
                {
                    const value_type * value = m_carriage - 1;

                    if( value->element == nullptr || value->element == MENGINE_HASHTABLE_DUMMY_ELEMENT )
                    {
                        continue;
                    }

                    break;
                }
            }

        protected:
            value_type * m_carriage;
            value_type * m_end;
        };

    public:
        const_iterator begin() const
        {
            return const_iterator( m_buffer, m_buffer + m_capacity );
        }

        const_iterator end() const
        {
            return const_iterator( m_buffer + m_capacity, m_buffer + m_capacity );
        }

        const_reverse_iterator rbegin() const
        {
            return const_reverse_iterator( m_buffer + m_capacity, m_buffer );
        }

        const_reverse_iterator rend() const
        {
            return const_reverse_iterator( m_buffer, m_buffer );
        }

    protected:
        void increase_()
        {
            if( m_buffer == nullptr )
            {
                m_capacity = 8;
                m_buffer = Hashtable::newbuffer_( m_capacity );

                return;
            }

            size_type new_capacity = (m_capacity << 1);

            this->rebalance_( new_capacity );
        }

    protected:
        static value_type * newbuffer_( size_type _capacity )
        {
            value_type * new_buffer = Helper::allocateMemoryNT<value_type>( _capacity, "hashtable" );

            for( size_type index = 0; index != _capacity; ++index )
            {
                value_type * record = new_buffer + index;

                new(record) value_type;
            }

            return new_buffer;
        }

        void rebalance_( size_type _capacity )
        {
            m_size = 0;
            m_dummy = 0;

            size_type old_capacity = m_capacity;
            value_type * old_buffer = m_buffer;

            value_type * new_buffer = Hashtable::newbuffer_( _capacity );

            m_capacity = _capacity;
            m_buffer = new_buffer;

            for( size_type index = 0; index != old_capacity; ++index )
            {
                value_type * record = old_buffer + index;

                if( record->element == nullptr || record->element == MENGINE_HASHTABLE_DUMMY_ELEMENT )
                {
                    continue;
                }

                this->push_( record->hash, record->key, record->element );

                record->key = key_type();
                record->element = nullptr;
            }

            Helper::deallocateMemory( old_buffer, "hashtable" );
        }

        void push_( hash_type _hash, const key_type & _key, element_type_ptr _element )
        {
            uint64_t hash_mask = m_capacity - 1;
            uint64_t mask = (uint64_t)_hash & hash_mask;

            for( uint64_t perturb = (uint64_t)_hash; ; perturb >>= MENGINE_HASHTABLE_PERTURB_SHIFT )
            {
                value_type * record = m_buffer + (mask & hash_mask);

                MENGINE_ASSERTION_FATAL( !(record->hash == _hash && record->key == _key && record->element != nullptr && record->element != MENGINE_HASHTABLE_DUMMY_ELEMENT), "hash push" );

                if( record->element == nullptr )
                {
                    record->hash = _hash;
                    record->key = _key;
                    record->element = _element;

                    ++m_size;

                    break;
                }

                if( record->element == MENGINE_HASHTABLE_DUMMY_ELEMENT )
                {
                    record->hash = _hash;
                    record->key = _key;
                    record->element = _element;

                    ++m_size;
                    --m_dummy;

                    break;
                }

                mask = (mask << 2) + mask + perturb + 1;
            }
        }

        element_type_ptr change_( hash_type _hash, const key_type & _key, element_type_ptr _element )
        {
            uint64_t hash_mask = m_capacity - 1;
            uint64_t mask = (uint64_t)_hash & hash_mask;

            for( uint64_t perturb = (uint64_t)_hash; ; perturb >>= MENGINE_HASHTABLE_PERTURB_SHIFT )
            {
                value_type * record = m_buffer + (mask & hash_mask);

                if( record->hash == _hash && record->key == _key )
                {
                    if( record->element == MENGINE_HASHTABLE_DUMMY_ELEMENT )
                    {
                        record->element = _element;

                        ++m_size;
                        --m_dummy;

                        return nullptr;
                    }

                    if( record->element != nullptr )
                    {
                        element_type_ptr prev = record->element;
                        record->element = _element;

                        return prev;
                    }
                }

                if( record->element == nullptr )
                {
                    record->hash = _hash;
                    record->key = _key;
                    record->element = _element;

                    ++m_size;

                    return nullptr;
                }

                mask = (mask << 2) + mask + perturb + 1;
            }
        }

        element_type_ptr pop_( hash_type _hash, const key_type & _key )
        {
            uint64_t hash_mask = m_capacity - 1;
            uint64_t mask = (uint64_t)_hash & hash_mask;

            for( uint64_t perturb = (uint64_t)_hash; ; perturb >>= MENGINE_HASHTABLE_PERTURB_SHIFT )
            {
                value_type * record = m_buffer + (mask & hash_mask);

                if( record->element == nullptr )
                {
                    return nullptr;
                }

                if( record->hash == _hash && record->key == _key && record->element != MENGINE_HASHTABLE_DUMMY_ELEMENT )
                {
                    element_type_ptr pop_element = record->element;

                    record->element = MENGINE_HASHTABLE_DUMMY_ELEMENT;

                    --m_size;
                    ++m_dummy;

                    return pop_element;
                }

                mask = (mask << 2) + mask + perturb + 1;
            }
        }

        element_type_ptr find_( hash_type _hash, const key_type & _key ) const
        {
            uint64_t hash_mask = m_capacity - 1;
            uint64_t mask = (uint64_t)_hash & hash_mask;

            for( uint64_t perturb = (uint64_t)_hash; ; perturb >>= MENGINE_HASHTABLE_PERTURB_SHIFT )
            {
                value_type * record = m_buffer + (mask & hash_mask);

                if( record->element == nullptr )
                {
                    return nullptr;
                }

                if( record->hash == _hash && record->key == _key )
                {
                    if( record->element == MENGINE_HASHTABLE_DUMMY_ELEMENT )
                    {
                        return nullptr;
                    }

                    return record->element;
                }

                mask = (mask << 2) + mask + perturb + 1;
            }
        }

        bool exist_( hash_type _hash, const key_type & _key ) const
        {
            uint64_t hash_mask = m_capacity - 1;
            uint64_t mask = (uint64_t)_hash & hash_mask;

            for( uint64_t perturb = (uint64_t)_hash; ; perturb >>= MENGINE_HASHTABLE_PERTURB_SHIFT )
            {
                value_type * record = m_buffer + (mask & hash_mask);

                if( record->element == nullptr )
                {
                    return false;
                }

                if( record->hash == _hash && record->key == _key )
                {
                    if( record->element == MENGINE_HASHTABLE_DUMMY_ELEMENT )
                    {
                        return false;
                    }

                    return true;
                }

                mask = (mask << 2) + mask + perturb + 1;
            }
        }

    protected:
        size_type m_size;
        size_type m_dummy;
        size_type m_capacity;

        value_type * m_buffer;
    };
}

#undef MENGINE_HASHTABLE_DUMMY_ELEMENT
#undef MENGINE_HASHTABLE_PERTURB_SHIFT