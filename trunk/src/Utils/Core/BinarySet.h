#   pragma once

#   include <vector>
#   include <algorithm>

namespace Menge
{
    template<class Key, class L = std::less<Key> >
    class BinarySet
    {
    public:
        typedef std::vector<Key> TBuffer;

        typedef typename TBuffer::iterator iterator;
        typedef typename TBuffer::const_iterator const_iterator;

    public:
        BinarySet()
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
        typedef std::pair<iterator, bool> TInserter;
        TInserter insert( const Key & _key )
        {
            iterator it_lower_bound = std::lower_bound( m_buffer.begin(), m_buffer.end(), _key, L() );

            if( it_lower_bound != m_buffer.end() )
            {
                if( L()( _key, *it_lower_bound ) == false && L()( *it_lower_bound, _key ) == false )
                {
                    return std::make_pair(it_lower_bound, false);
                }                
            }
            
            iterator it_insert = m_buffer.insert( it_lower_bound, _key );
            
            return std::make_pair(it_insert, true);
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
        bool has( const Key & _key ) const
        {
            const_iterator it_lower_bound = std::lower_bound( m_buffer.begin(), m_buffer.end(), _key, L() );

            const_iterator it_end = this->end();

            if( it_lower_bound == it_end )
            {
                return false;
            }

            if( L()( _key, *it_lower_bound ) == true )
            {
                return false;
            }

            return true;
        }

    protected:
        TBuffer m_buffer;
    };
}