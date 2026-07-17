#include "Arena3DLocalCommandTransport.h"

#include "Config/StdUtility.h"

//////////////////////////////////////////////////////////////////////////
namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    void LocalCommandTransport::clear()
    {
        m_toServer.clear();
        m_toClient.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void LocalCommandTransport::sendCommand( const CommandEnvelope & _command )
    {
        m_toServer.emplace_back( _command );
    }
    //////////////////////////////////////////////////////////////////////////
    VectorCommandEnvelope LocalCommandTransport::takeServerCommands()
    {
        VectorCommandEnvelope result;
        result.swap( m_toServer );
        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void LocalCommandTransport::publishServerFrame( ServerFrame _frame )
    {
        m_toClient.emplace_back( Mengine::StdUtility::move( _frame ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool LocalCommandTransport::takeClientFrame( ServerFrame * _frame )
    {
        if( m_toClient.empty() == true )
        {
            return false;
        }
        *_frame = Mengine::StdUtility::move( m_toClient.front() );
        m_toClient.pop_front();
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
