#include "DevToDebugPropertyContent.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugPropertyContent::DevToDebugPropertyContent()
        : m_invalidate( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugPropertyContent::~DevToDebugPropertyContent()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugPropertyContent::addState( const ConstString & _id, const String & _text )
    {
        StateDesc desc;
        desc.id = _id;
        desc.text = _text;

        m_states.push_back( desc );

        m_invalidate = true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugPropertyContent::fillPropertyJson( const ConstString & _name, jpp::object & _jwidget, bool _force ) const
    {
        if( _force == false )
        {
            if( m_invalidate == false )
            {
                return false;
            }
        }

        jpp::array jcontent = jpp::make_array();

        for( const StateDesc & desc : m_states )
        {
            jpp::object jstate = jpp::make_object();

            jstate.set( "value", desc.id );
            jstate.set( "text", desc.text );

            jcontent.push_back( jstate );
        }

        _jwidget.set( _name.c_str(), jcontent );

        m_invalidate = false;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}