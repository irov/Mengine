#	include "ConstStringHolder.h"

#   include "Config/Typedef.h"

#   include "Core/Pool.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////			
    inline static size_t s_make_hash( const char * _data )
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
    ConstStringHolder::ConstStringHolder()
        : m_reference(0)
        , m_hash(0)
        , m_size(0)
    {
        m_owner = this;               
    }
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolder & ConstStringHolder::operator = ( const ConstStringHolder & _holder )
    {
        (void)_holder;

        return *this;
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolder::setup( const char * _data, size_t _size )
    {
        m_hash = s_make_hash( _data );
        m_size = _size;
    }
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
    void ConstStringHolder::combine( ConstStringHolder * _holder )
    {
        if( m_owner->m_reference > _holder->m_owner->m_reference )
        {
            this->combine_from( _holder, this );
        }
        else
        {
            this->combine_from( this, _holder );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolder::combine_owner( ConstStringHolder * _out )
    {
        this->m_owner = _out->m_owner;
        this->m_reference >>= 1;

        _out->m_owner->m_reference += this->m_reference;
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolder::combine_other( ConstStringHolder * _out )
    {
        this->m_owner->m_reference -= this->m_reference;
        this->m_owner = _out->m_owner;

        _out->m_owner->m_reference += this->m_reference;
    }
    //////////////////////////////////////////////////////////////////////////
    class ConstStringHolder::ForeachCombineOwner
    {
    public:
        ForeachCombineOwner( ConstStringHolder * _out )
            : m_out(_out)
        {
        }

    public:
        void operator () ( IntrusiveLinked<ConstStringHolder> * _linked ) const
        {
            ConstStringHolder * elem = static_cast<ConstStringHolder *>(_linked);

            elem->combine_owner( m_out );
        }

    protected:
        ConstStringHolder * m_out;
    };
    //////////////////////////////////////////////////////////////////////////
    class ConstStringHolder::ForeachCombineOther
    {
    public:
        ForeachCombineOther( ConstStringHolder * _out )
            : m_out(_out)
        {
        }

    public:
        void operator () ( IntrusiveLinked<ConstStringHolder> * _linked ) const
        {
            ConstStringHolder * elem = static_cast<ConstStringHolder *>(_linked);

            elem->combine_other( m_out );
        }

    protected:
        ConstStringHolder * m_out;
    };
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolder::combine_from( ConstStringHolder * _from, ConstStringHolder * _out )
    {
        _from->m_owner->releaseString();

        if( _from->unique() == true )
        {
            ForeachCombineOwner combineowner(_out);
            _from->foreach_self( combineowner );
        }
        else
        {
            ConstStringHolder * from_owner = _from->m_owner;

            ForeachCombineOther combineother(_out);
            from_owner->foreach_other( combineother );

            if( from_owner->m_reference == 0 )
            {
                from_owner->destroy();
            }
            else
            {
                ForeachCombineOwner combineowner(_out);
                from_owner->foreach_self( combineowner );						
            }
        }

        _from->linkall( _out );
    }
}