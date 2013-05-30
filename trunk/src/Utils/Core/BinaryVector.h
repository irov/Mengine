#   pragma once

#   include <vector>
#   include <algorithm>

namespace Menge
{
    template<class Key, class T, class L = std::less<Key>>
    class BinaryVector
    {
    public:
        typedef std::pair<Key, T> TBinaryPair;
        typedef std::vector<TBinaryPair> TBuffer;

        typedef typename TBuffer::iterator iterator;
        typedef typename TBuffer::const_iterator const_iterator;

    protected:
        struct BinaryVectorLess
        {
        public:
            bool operator () ( const TBinaryPair & _left, const TBinaryPair & _right ) const
            {
                return L()( _left.first, _right.first );
            }
        };

    public:
        BinaryVector()
        {
        }

    public:
        void reserve( size_t _size )
        {
            m_buffer.reserve( _size );
        }

        size_t size() const
        {
            size_t size = m_buffer.size();

            return size;
        }

        void clear()
        {
            m_buffer.clear();
        }

    public:
        iterator begin()
        {
            iterator it = m_buffer.begin();
            
            return it;
        }

        iterator end()
        {
            iterator it = m_buffer.end();

            return it;
        }

        const_iterator begin() const
        {
            const_iterator it = m_buffer.begin();

            return it;
        }

        const_iterator end() const
        {
            const_iterator it = m_buffer.end();

            return it;
        }

    public:
        iterator insert( const TBinaryPair & _pair )
        {
            iterator it_lower_bound = std::lower_bound( m_buffer.begin(), m_buffer.end(), _pair, BinaryVectorLess() );

            iterator it_insert = m_buffer.insert( it_lower_bound, _pair );
            
            return it_insert;
        }

        void erase( iterator _erase )
        {
            m_buffer.erase( _erase );
        }

        bool erase( const Key & _key )
        {
            iterator it_found = this->find( _key );
            
            if( it_found == this->end() )
            {
                return false;
            }

            this->erase( it_found );
            
            return true;
        }
        
    public:
        bool has( const Key & _key, T ** _it )
        {
            iterator it_found = this->find( _key );

            if( it_found == this->end() )
            {
                return false;
            }

            if( _it != nullptr )
            {
                *_it = &it_found->second;
            }

            return true;
        }

        bool has( const Key & _key, T ** _it ) const
        {
            const_iterator it_found = this->find( _key );

            if( it_found == this->end() )
            {
                return false;
            }

            if( _it != nullptr )
            {
                *_it = &it_found->second;
            }

            return true;
        }

    public:
        iterator find( const Key & _key )
        {
            TBinaryPair key_value(_key, T());
            iterator it_lower_bound = std::lower_bound( m_buffer.begin(), m_buffer.end(), key_value, BinaryVectorLess() );

            iterator it_end = this->end();

            if( it_lower_bound == it_end )
            {
                return it_end;
            }

            if( BinaryVectorLess()( key_value, *it_lower_bound ) == true )
            {
                return it_end;
            }

            return it_lower_bound;
        }
        
        const_iterator find( const Key & _key ) const
        {
            TBinaryPair key_value(_key, T());
            const_iterator it_lower_bound = std::lower_bound( m_buffer.begin(), m_buffer.end(), key_value, BinaryVectorLess() );

            const_iterator it_end = this->end();

            if( it_lower_bound == it_end )
            {
                return it_end;
            }

            if( BinaryVectorLess()( key_value, *it_lower_bound ) == true )
            {
                return it_end;
            }

            return it_lower_bound;
        }

    protected:
        TBuffer m_buffer;
    };
}