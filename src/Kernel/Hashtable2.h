#pragma once

#include "Kernel/Hashgen.h"
#include "Kernel/IntrusivePtr.h"
#include "Kernel/MemoryAllocator.h"
#include "Kernel/Assertion.h"

namespace Mengine
{
    template<class K1, class K2, class T, class H1 = Hashgen<K1>, class H2 = Hashgen<K2>>
    class Hashtable2
    {
    public:
        typedef uint32_t size_type;
        typedef HashType hash_type;
        typedef K1 key_type1;
        typedef K2 key_type2;
        typedef T element_type_ptr;
        typedef H1 hashgen_type1;
        typedef H2 hashgen_type2;
        typedef typename element_type_ptr::value_type element_type;

        struct value_type
        {
            value_type()
                : hash( 0 )
            {
            }


            hash_type hash;
            key_type1 key1;
            key_type2 key2;
            element_type_ptr element;
        };

    public:
        Hashtable2()
            : m_size( 0 )
            , m_capacity( 0 )
            , m_buffer( nullptr )
        {
        }

        ~Hashtable2()
        {
            this->clear();

            Helper::freeArrayT( m_buffer );
        }

    protected:
        static hash_type get_hash_key( const key_type1 & _key1, const key_type2 & _key2 )
        {
            hash_type hash1 = hashgen_type1()(_key1);
            hash_type hash2 = hashgen_type2()(_key2);
            hash_type hash = hash1 ^ hash2;

            return hash;
        }

    public:
        const element_type_ptr & emplace( const key_type1 & _key1, const key_type2 & _key2, const element_type_ptr & _element )
        {
            uint32_t test_size = m_size * 3 + 1;
            uint32_t test_capacity = m_capacity * 2;

            if( test_size > test_capacity )
            {
                this->increase_();
            }

            hash_type hash = get_hash_key( _key1, _key2 );

            const element_type_ptr & element = Hashtable2::push_( m_buffer, m_capacity, hash, _key1, _key2, _element );

            ++m_size;

            return element;
        }

        element_type_ptr change( const key_type1 & _key1, const key_type2 & _key2, const element_type_ptr & _element )
        {
            uint32_t test_size = m_size * 3 + 1;
            uint32_t test_capacity = m_capacity * 2;

            if( test_size > test_capacity )
            {
                this->increase_();
            }

            hash_type hash = get_hash_key( _key1, _key2 );

            element_type_ptr prev = Hashtable2::change_( m_buffer, m_capacity, hash, _key1, _key2, _element );

            if( prev == nullptr )
            {
                ++m_size;
            }

            return prev;
        }

        element_type_ptr erase( const key_type1 & _key1, const key_type2 & _key2 )
        {
            if( m_size == 0 )
            {
                return nullptr;
            }

            hash_type hash = get_hash_key( _key1, _key2 );

            element_type_ptr element = Hashtable2::pop_( m_buffer, m_capacity, hash, _key1, _key2 );

            if( element != nullptr )
            {
                --m_size;
            }

            return element;
        }

        const element_type_ptr & find( const key_type1 & _key1, const key_type2 & _key2 ) const
        {
            if( m_size == 0 )
            {
                return element_type_ptr::none();
            }

            hash_type hash = get_hash_key( _key1, _key2 );

            const element_type_ptr & element = Hashtable2::find_( m_buffer, m_capacity, hash, _key1, _key2 );

            return element;
        }

        bool exist( const key_type1 & _key1, const key_type2 & _key2 ) const
        {
            if( m_size == 0 )
            {
                return false;
            }

            hash_type hash = get_hash_key( _key1, _key2 );

            bool result = Hashtable2::exist_( m_buffer, m_capacity, hash, _key1, _key2 );

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

                if( record->element.get_base() == nullptr || record->element.get_base() == reinterpret_cast<const element_type *>(~0ULL) )
                {
                    continue;
                }

                record->element = nullptr;
                record->key1 = key_type1();
                record->key2 = key_type2();
            }

            m_size = 0;
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
                    if( m_carriage->element.get_base() == nullptr || m_carriage->element.get_base() == reinterpret_cast<const element_type *>(~0ULL) )
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

                    if( value->element.get_base() == nullptr || value->element.get_base() == reinterpret_cast<const element_type *>(~0ULL) )
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
                m_buffer = this->newbuffer_( m_capacity );

                return;
            }

            size_type new_capacity = (m_capacity << 1);

            this->rebalance_( new_capacity );
        }

    protected:
        static value_type * newbuffer_( size_type _capacity )
        {
            value_type * new_buffer = Helper::allocateArrayT<value_type>( _capacity );

            for( size_type index = 0; index != _capacity; ++index )
            {
                value_type * record = new_buffer + index;

                new(record) value_type;
            }

            return new_buffer;
        }

        void rebalance_( size_type _capacity )
        {
            size_type old_capacity = m_capacity;
            value_type * old_buffer = m_buffer;

            value_type * new_buffer = this->newbuffer_( _capacity );

            for( size_type index = 0; index != old_capacity; ++index )
            {
                value_type * record = old_buffer + index;

                if( record->element.get_base() == nullptr || record->element.get_base() == reinterpret_cast<const element_type *>(~0ULL) )
                {
                    continue;
                }

                Hashtable2::push_( new_buffer, _capacity, record->hash, record->key1, record->key2, record->element );

                record->key1 = key_type1();
                record->key2 = key_type2();
                record->element = nullptr;
            }

            Helper::freeArrayT( old_buffer );

            m_capacity = _capacity;
            m_buffer = new_buffer;
        }

        static const element_type_ptr & push_( value_type * _buffer, size_type _capacity, hash_type _hash, const key_type1 & _key1, const key_type2 & _key2, const element_type_ptr & _element )
        {
            size_type hash_mask = _capacity - 1;
            size_type mask = (size_type)_hash;

            for( hash_type probe = _hash; ; probe >>= 5 )
            {
                value_type * record = _buffer + (mask & hash_mask);

                MENGINE_ASSERTION_FATAL( !(record->hash == _hash && record->key1 == _key1 && record->key2 == _key2 && record->element.get_base() != nullptr && record->element.get_base() != reinterpret_cast<const element_type *>(~0ULL)), "hash push" );

                if( record->element.get_base() == nullptr )
                {
                    record->hash = _hash;
                    record->key1 = _key1;
                    record->key2 = _key2;
                    record->element = _element;

                    return record->element;
                }

                if( record->element.get_base() == reinterpret_cast<const element_type *>(~0ULL) )
                {
                    record->hash = _hash;
                    record->key1 = _key1;
                    record->key2 = _key2;
                    record->element.set( nullptr );
                    record->element = _element;

                    return record->element;
                }

                mask = (mask << 2) + mask + (size_type)probe + 1;
            }
        }

        static element_type_ptr change_( value_type * _buffer, size_type _capacity, hash_type _hash, const key_type1 & _key1, const key_type2 & _key2, const element_type_ptr & _element )
        {
            size_type hash_mask = _capacity - 1;
            size_type mask = (size_type)_hash;

            for( hash_type probe = _hash; ; probe >>= 5 )
            {
                value_type * record = _buffer + (mask & hash_mask);

                if( record->hash == _hash && record->key1 == _key1 && record->key2 == _key2 )
                {
                    if( record->element.get_base() == reinterpret_cast<const element_type *>(~0ULL) )
                    {
                        record->element.set( nullptr );
                        record->element = _element;

                        return nullptr;
                    }

                    if( record->element.get_base() != nullptr )
                    {
                        element_type_ptr prev = std::move( record->element );
                        record->element = _element;

                        return std::move( prev );
                    }
                }

                if( record->element.get_base() == nullptr )
                {
                    record->hash = _hash;
                    record->key1 = _key1;
                    record->key2 = _key2;
                    record->element = _element;

                    return nullptr;
                }

                mask = (mask << 2) + mask + (size_type)probe + 1;
            }
        }

        static element_type_ptr pop_( value_type * _buffer, size_type _capacity, hash_type _hash, const key_type1 & _key1, const key_type2 & _key2 )
        {
            size_type hash_mask = _capacity - 1;
            size_type mask = (size_type)_hash;

            for( hash_type probe = _hash; ; probe >>= 5 )
            {
                value_type * record = _buffer + (mask & hash_mask);

                if( record->element.get_base() == nullptr )
                {
                    return nullptr;
                }

                if( record->hash == _hash && record->key1 == _key1 && record->key2 == _key2 && record->element.get_base() != reinterpret_cast<const element_type *>(~0ULL) )
                {
                    element_type_ptr pop_element = std::move( record->element );

                    record->element.set( reinterpret_cast<element_type *>(~0ULL) );

                    return std::move( pop_element );
                }

                mask = (mask << 2) + mask + (size_type)probe + 1;
            }
        }

        static const element_type_ptr & find_( value_type * _buffer, size_type _capacity, hash_type _hash, const key_type1 & _key1, const key_type2 & _key2 )
        {
            size_type hash_mask = _capacity - 1;
            size_type mask = (size_type)_hash;

            for( hash_type probe = _hash; ; probe >>= 5 )
            {
                value_type * record = _buffer + (mask & hash_mask);

                if( record->element.get_base() == nullptr )
                {
                    return element_type_ptr::none();
                }

                if( record->hash == _hash && record->key1 == _key1 && record->key2 == _key2 )
                {
                    if( record->element.get_base() == reinterpret_cast<const element_type *>(~0ULL) )
                    {
                        return element_type_ptr::none();
                    }

                    return record->element;
                }

                mask = (mask << 2) + mask + (size_type)probe + 1;
            }
        }

        static bool exist_( value_type * _buffer, size_type _capacity, hash_type _hash, const key_type1 & _key1, const key_type2 & _key2 )
        {
            size_type hash_mask = _capacity - 1;
            size_type mask = (size_type)_hash;

            for( hash_type probe = _hash; ; probe >>= 5 )
            {
                value_type * record = _buffer + (mask & hash_mask);

                if( record->element.get_base() == nullptr )
                {
                    return false;
                }

                if( record->hash == _hash && record->key1 == _key1 && record->key2 == _key2 )
                {
                    if( record->element.get_base() == reinterpret_cast<const element_type *>(~0ULL) )
                    {
                        return false;
                    }

                    return true;
                }

                mask = (mask << 2) + mask + (size_type)probe + 1;
            }
        }

    protected:
        size_type m_size;
        size_type m_capacity;

        value_type * m_buffer;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class K1, class K2, class T, class H1, class H2>
    class Hashtable2<K1, K2, T *, H1, H2>
    {
    public:
        typedef uint32_t size_type;
        typedef HashType hash_type;
        typedef K1 key_type1;
        typedef K2 key_type2;
        typedef T * element_type_ptr;
        typedef H1 hashgen_type1;
        typedef H2 hashgen_type2;
        typedef T element_type;

        struct value_type
        {
            value_type()
                : hash( 0 )
                , element( nullptr )
            {
            }

            hash_type hash;
            key_type1 key1;
            key_type2 key2;
            element_type_ptr element;
        };

    public:
        Hashtable2()
            : m_size( 0 )
            , m_capacity( 0 )
            , m_buffer( nullptr )
        {
        }

        ~Hashtable2()
        {
            this->clear();

            Helper::freeArrayT( m_buffer );
        }

    protected:
        static hash_type get_hash_key( const key_type1 & _key1, const key_type2 & _key2 )
        {
            hash_type hash1 = hashgen_type1()(_key1);
            hash_type hash2 = hashgen_type2()(_key2);
            hash_type hash = hash1 ^ hash2;

            return hash;
        }

    public:
        element_type_ptr emplace( const key_type1 & _key1, const key_type2 & _key2, element_type_ptr _element )
        {
            uint32_t test_size = m_size * 3 + 1;
            uint32_t test_capacity = m_capacity * 2;

            if( test_size > test_capacity )
            {
                this->increase_();
            }

            hash_type hash = get_hash_key( _key1, _key2 );

            Hashtable2::push_( m_buffer, m_capacity, hash, _key1, _key2, _element );

            ++m_size;

            return _element;
        }

        element_type_ptr change( const key_type1 & _key1, const key_type2 & _key2, element_type_ptr _element )
        {
            uint32_t test_size = m_size * 3 + 1;
            uint32_t test_capacity = m_capacity * 2;

            if( test_size > test_capacity )
            {
                this->increase_();
            }

            hash_type hash = get_hash_key( _key1, _key2 );

            element_type_ptr prev = Hashtable2::change_( m_buffer, m_capacity, hash, _key1, _key2, _element );

            if( prev == nullptr )
            {
                ++m_size;
            }

            return prev;
        }

        element_type_ptr erase( const key_type1 & _key1, const key_type2 & _key2 )
        {
            if( m_size == 0 )
            {
                return nullptr;
            }

            hash_type hash = get_hash_key( _key1, _key2 );

            element_type_ptr element = Hashtable2::pop_( m_buffer, m_capacity, hash, _key1, _key2 );

            if( element != nullptr )
            {
                --m_size;
            }

            return element;
        }

        element_type_ptr find( const key_type1 & _key1, const key_type2 & _key2 ) const
        {
            if( m_size == 0 )
            {
                return nullptr;
            }

            hash_type hash = get_hash_key( _key1, _key2 );

            element_type_ptr element = Hashtable2::find_( m_buffer, m_capacity, hash, _key1, _key2 );

            return element;
        }

        bool exist( const key_type1 & _key1, const key_type2 & _key2 ) const
        {
            if( m_size == 0 )
            {
                return false;
            }

            hash_type hash = get_hash_key( _key1, _key2 );

            bool result = Hashtable2::exist_( m_buffer, m_capacity, hash, _key1, _key2 );

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

                if( record->element == nullptr || record->element == reinterpret_cast<const element_type *>(~0ULL) )
                {
                    continue;
                }

                record->element = nullptr;
                record->key1 = key_type1();
                record->key2 = key_type2();
            }

            m_size = 0;
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

                    if( value->element == nullptr || value->element == reinterpret_cast<const element_type *>(~0ULL) )
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

                    if( value->element == nullptr || value->element == reinterpret_cast<const element_type *>(~0ULL) )
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
                m_buffer = this->newbuffer_( m_capacity );

                return;
            }

            size_type new_capacity = (m_capacity << 1);

            this->rebalance_( new_capacity );
        }

    protected:
        static value_type * newbuffer_( size_type _capacity )
        {
            value_type * new_buffer = Helper::allocateArrayT<value_type>( _capacity );

            for( size_type index = 0; index != _capacity; ++index )
            {
                value_type * record = new_buffer + index;

                new(record) value_type;
            }

            return new_buffer;
        }

        void rebalance_( size_type _capacity )
        {
            size_type old_capacity = m_capacity;
            value_type * old_buffer = m_buffer;

            value_type * new_buffer = this->newbuffer_( _capacity );

            for( size_type index = 0; index != old_capacity; ++index )
            {
                value_type * record = old_buffer + index;

                if( record->element == nullptr || record->element == reinterpret_cast<const element_type *>(~0ULL) )
                {
                    continue;
                }

                Hashtable2::push_( new_buffer, _capacity, record->hash, record->key1, record->key2, record->element );

                record->key1 = key_type1();
                record->key2 = key_type2();
                record->element = nullptr;
            }

            Helper::freeArrayT( old_buffer );

            m_capacity = _capacity;
            m_buffer = new_buffer;
        }

        static void push_( value_type * _buffer, size_type _capacity, hash_type _hash, const key_type1 & _key1, const key_type2 & _key2, element_type_ptr _element )
        {
            size_type hash_mask = _capacity - 1;
            size_type mask = (size_type)_hash;

            for( hash_type probe = _hash; ; probe >>= 5 )
            {
                value_type * record = _buffer + (mask & hash_mask);

                MENGINE_ASSERTION_FATAL( !(record->hash == _hash && record->key1 == _key1 && record->key2 == _key2 && record->element != nullptr && record->element != reinterpret_cast<const element_type *>(~0ULL)), "hash push" );

                if( record->element == nullptr || record->element == reinterpret_cast<const element_type *>(~0ULL) )
                {
                    record->hash = _hash;
                    record->key1 = _key1;
                    record->key2 = _key2;
                    record->element = _element;

                    break;
                }

                mask = (mask << 2) + mask + (size_type)probe + 1;
            }
        }

        static element_type_ptr change_( value_type * _buffer, size_type _capacity, hash_type _hash, const key_type1 & _key1, const key_type2 & _key2, element_type_ptr _element )
        {
            size_type hash_mask = _capacity - 1;
            size_type mask = (size_type)_hash;

            for( hash_type probe = _hash; ; probe >>= 5 )
            {
                value_type * record = _buffer + (mask & hash_mask);

                if( record->hash == _hash && record->key1 == _key1 && record->key2 == _key2 )
                {
                    if( record->element == reinterpret_cast<const element_type *>(~0ULL) )
                    {
                        record->element = _element;

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
                    record->key1 = _key1;
                    record->key2 = _key2;
                    record->element = _element;

                    return nullptr;
                }

                mask = (mask << 2) + mask + (size_type)probe + 1;
            }
        }

        static element_type_ptr pop_( value_type * _buffer, size_type _capacity, hash_type _hash, const key_type1 & _key1, const key_type2 & _key2 )
        {
            size_type hash_mask = _capacity - 1;
            size_type mask = (size_type)_hash;

            for( hash_type probe = _hash; ; probe >>= 5 )
            {
                value_type * record = _buffer + (mask & hash_mask);

                if( record->element == nullptr )
                {
                    return nullptr;
                }

                if( record->hash == _hash && record->key1 == _key1 && record->key2 == _key2 && record->element != reinterpret_cast<const element_type *>(~0ULL) )
                {
                    element_type_ptr pop_element = record->element;

                    record->element = reinterpret_cast<element_type *>(~0ULL);

                    return pop_element;
                }

                mask = (mask << 2) + mask + (size_type)probe + 1;
            }
        }

        static element_type_ptr find_( value_type * _buffer, size_type _capacity, hash_type _hash, const key_type1 & _key1, const key_type2 & _key2 )
        {
            size_type hash_mask = _capacity - 1;
            size_type mask = (size_type)_hash;

            for( hash_type probe = _hash; ; probe >>= 5 )
            {
                value_type * record = _buffer + (mask & hash_mask);

                if( record->element == nullptr )
                {
                    return nullptr;
                }

                if( record->hash == _hash && record->key1 == _key1 && record->key2 == _key2 )
                {
                    if( record->element == reinterpret_cast<const element_type *>(~0ULL) )
                    {
                        return nullptr;
                    }

                    return record->element;
                }

                mask = (mask << 2) + mask + (size_type)probe + 1;
            }
        }

        static bool exist_( value_type * _buffer, size_type _capacity, hash_type _hash, const key_type1 & _key1, const key_type2 & _key2 )
        {
            size_type hash_mask = _capacity - 1;
            size_type mask = (size_type)_hash;

            for( hash_type probe = _hash; ; probe >>= 5 )
            {
                value_type * record = _buffer + (mask & hash_mask);

                if( record->element == nullptr )
                {
                    return false;
                }

                if( record->hash == _hash && record->key1 == _key1 && record->key2 == _key2 )
                {
                    if( record->element == reinterpret_cast<const element_type *>(~0ULL) )
                    {
                        return false;
                    }

                    return true;
                }

                mask = (mask << 2) + mask + (size_type)probe + 1;
            }
        }

    protected:
        size_type m_size;
        size_type m_capacity;

        value_type * m_buffer;
    };
}