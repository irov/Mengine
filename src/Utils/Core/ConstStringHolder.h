#	pragma once

#   include "Config/Typedef.h"

#	include "Core/IntrusivePtr.h"
#	include "Core/IntrusiveLinked.h"

#	include "Factory/Factorable.h"

namespace Menge
{
    namespace Detail
    {
        template<class T>
        int cs_strcmp( const T * _left, const T * _right );

        template<class T>
        size_t cs_make_hash( const T * _data );
    }

    template<class T>
    class ConstStringHolderT
        : public IntrusiveLinked< ConstStringHolderT<T> >
        , public Factorable
    {
    protected:
        ConstStringHolderT()
            : m_reference(0)
            , m_data(nullptr)
            , m_hash(0)
            , m_size(0)
        {
            m_owner = this;
        }

    private:
        ConstStringHolderT & operator = ( const ConstStringHolderT & _holder )
        {
            (void)_holder;

            return *this;
        }

    public:
        inline const T * c_str() const
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
        const T * _c_str() const
        {
            return m_data;
        }

    protected:
        virtual void releaseString() = 0;

    public:
        inline ConstStringHolderT * owner() const
        {
            return m_owner;
        }

        inline size_t hash() const
        {
            return m_hash;
        }

    public:
        bool less( ConstStringHolderT * _holder )
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

            const T * left = this->c_str();
            const T * right = _holder->c_str();

            int res = Detail::cs_strcmp<T>( left, right );

            if( res == 0 )
            {
                this->combine( _holder );

                return false;
            }

            return res < 0;
        }

        bool equal( ConstStringHolderT * _holder )
        {
            if( m_owner == _holder->m_owner )
            {
                return true;
            }

            if( m_hash != _holder->m_hash )
            {
                return false;
            }

            const T * left = this->c_str();
            const T * right = _holder->c_str();

            if( Detail::cs_strcmp( left, right ) != 0 )
            {
                return false;
            }

            this->combine( _holder );

            return true;
        }

    public:
        void combine( ConstStringHolderT * _holder )
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

    protected:        
        class ForeachCombineOwner
        {
        public:
            ForeachCombineOwner( ConstStringHolderT * _out )
                : m_out(_out)
            {
            }

        public:
            void operator () ( IntrusiveLinked<ConstStringHolderT> * _linked ) const
            {
                ConstStringHolderT * elem = static_cast<ConstStringHolderT *>(_linked);

                elem->combine_owner( m_out );
            }

        protected:
            ConstStringHolderT * m_out;
        };
        
        class ForeachCombineOther
        {
        public:
            ForeachCombineOther( ConstStringHolderT * _out )
                : m_out(_out)
            {
            }

        public:
            void operator () ( IntrusiveLinked<ConstStringHolderT> * _linked ) const
            {
                ConstStringHolderT * elem = static_cast<ConstStringHolderT *>(_linked);

                elem->combine_other( m_out );
            }

        protected:
            ConstStringHolderT * m_out;
        };

        void combine_owner( ConstStringHolderT * _out )
        {
            this->m_owner = _out->m_owner;
            this->m_reference >>= 1;

            _out->m_owner->m_reference += this->m_reference;
        }

        void combine_other( ConstStringHolderT * _out )
        {
            this->m_owner->m_reference -= this->m_reference;
            this->m_owner = _out->m_owner;

            _out->m_owner->m_reference += this->m_reference;
        }

        void combine_from( ConstStringHolderT * _from, ConstStringHolderT * _out )
        {
            _from->m_owner->releaseString();

            if( _from->unique() == true )
            {
                ForeachCombineOwner combineowner(_out);
                _from->foreach_self( combineowner );
            }
            else
            {
                ConstStringHolderT * from_owner = _from->m_owner;

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

    protected:
        friend inline void intrusive_ptr_add_ref( ConstStringHolderT * _ptr )
        {
            ++_ptr->m_reference;
            ++_ptr->m_owner->m_reference;
        }

        friend inline void intrusive_ptr_release( ConstStringHolderT * _ptr )
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
        void setup( const T * _data, size_t _size )
        {
            m_data = _data;
            m_hash = Detail::cs_make_hash( m_data );
            m_size = _size;
        }

    protected:
        size_t m_reference;

        const T * m_data;

        size_t m_hash;
        size_t m_size;

        mutable ConstStringHolderT * m_owner;
    };
}