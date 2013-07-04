#	include "ConstStringHolder.h"

#   include <string.h>

namespace Menge
{    
    //////////////////////////////////////////////////////////////////////////
    bool ConstStringHolder::less( ConstStringHolder * _holder )
    {
        if( m_hash < _holder->m_hash )
        {
            return true;
        }

        if( m_hash > _holder->m_hash )
        {
            return false;
        }

        if( m_owner == _holder->m_owner )
        {
            return false;
        }

        const char * left = this->c_str();
        const char * right = _holder->c_str();

        int res = strcmp( left, right );

        if( res == 0 )
        {
            this->combine( _holder );

            return false;
        }

        return res < 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ConstStringHolder::equal( ConstStringHolder * _holder )
    {
        if( m_owner == _holder->m_owner )
        {
            return true;
        }

        if( m_hash != _holder->m_hash )
        {
            return false;
        }

        const char * left = this->c_str();
        const char * right = _holder->c_str();

        if( strcmp( left, right ) != 0 )
        {
            return false;
        }

        this->combine( _holder );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static size_t s_make_hash( const char * _data )
    {
        const char * str = _data;

        size_t hash = 5381;
        int c;

        while(c = *str++)
        {
            hash = ((hash << 5) + hash) + c;
        }

        return hash;
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolder::setup( const char * _data, size_t _size )
    {
        m_data = _data;
        m_hash = s_make_hash( m_data );
        m_size = _size;
    }
}