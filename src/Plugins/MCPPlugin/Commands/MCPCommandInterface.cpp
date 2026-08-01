#include "MCPCommandInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPCommandResponse::MCPCommandResponse()
    {
        jpp::object value = jpp::make_object();

        result = value;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & MCPCommandInterface::getName() const
    {
        const ConstString & name = m_name;

        return name;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPCommandInterface::availableWhilePaused( const jpp::object & _params ) const
    {
        MENGINE_UNUSED( _params );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPCommandInterface::update()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPCommandInterface::cancel( uint32_t _requestId )
    {
        MENGINE_UNUSED( _requestId );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPCommandInterface::disconnect()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPCommandInterface::setName_( const ConstString & _name )
    {
        if( _name.empty() == true || m_name.empty() == false )
        {
            return false;
        }

        m_name = _name;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
