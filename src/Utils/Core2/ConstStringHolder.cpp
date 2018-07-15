#include "ConstStringHolder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolder::ConstStringHolder()
        : m_data( nullptr )
        , m_hash( 0 )
        , m_size( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolder::ConstStringHolder( const ConstStringHolder & _holder )
        : m_data( _holder.m_data )
        , m_size( _holder.m_size )
        , m_hash( _holder.m_hash )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolder::~ConstStringHolder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolder::setup( const value_type * _data, size_type _size, hash_type _hash )
    {
        m_data = _data;
        m_size = _size;
        m_hash = _hash;
    }
}