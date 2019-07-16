#include "Tags.h"

#include <algorithm>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Tags::Tags()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Tags::~Tags()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Tags::addTag( const ConstString & _tag )
    {
        m_values.emplace_back( _tag );
    }
    //////////////////////////////////////////////////////////////////////////
    void Tags::removeTag( const ConstString & _tag )
    {
        VectorConstString::iterator it_found = std::find( m_values.begin(), m_values.end(), _tag );

        if( it_found == m_values.end() )
        {
            return;
        }

        m_values.erase( it_found );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Tags::hasTag( const ConstString & _tag ) const
    {
        VectorConstString::const_iterator it_found = std::find( m_values.begin(), m_values.end(), _tag );

        return it_found != m_values.end();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Tags::hasTags( const Tags & _tag ) const
    {
        for( const ConstString & tag : _tag.m_values )
        {
            if( this->hasTag( tag ) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Tags::empty() const
    {
        return m_values.empty();
    }
    //////////////////////////////////////////////////////////////////////////
    void Tags::clear()
    {
        m_values.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorConstString & Tags::getValues() const
    {
        return m_values;
    }
}