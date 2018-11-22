#include "NodeDebuggerService.h"

#include "Interface/OptionsInterface.h"
#include "Interface/PlayerInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/RenderServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/FactoryServiceInterface.h"
#include "Interface/SocketSystemInterface.h"

#include "Plugins/AstralaxParticlePlugin/AstralaxInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/ThreadTask.h"
#include "Kernel/SchedulerHelper.h"

#include "Config/Stringstream.h"

#include "pybind/pybind.hpp"
#include "stdex/allocator_report.h"

#include <iomanip>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( NodeDebuggerService, Mengine::NodeDebuggerService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    NodeDebuggerService::NodeDebuggerService()
        : m_serverState( NodeDebuggerServerState::Invalid )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    NodeDebuggerService::~NodeDebuggerService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerService::_dependencyService()
    {
        SERVICE_DEPENDENCY( NodeDebuggerService, PlayerServiceInterface );
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeDebuggerService::_initializeService()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerService::_finalizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeDebuggerService::onWork( uint32_t )
    {
        if( m_socketListen == nullptr )
        {
            return false;
        }

        switch( m_serverState )
        {
            case NodeDebuggerServerState::WaitingForClient:
            {
                if( !m_socketListen->waitForClient() )
                {
                    m_serverState = NodeDebuggerServerState::Invalid;
                    return false;
                }

                m_serverState = NodeDebuggerServerState::Connected;
            } break;

            case NodeDebuggerServerState::Connected:
            {
                Mengine::String resp = "HTTP/1.1 200 OK\n"\
                    "Content-Length: 13\n"\
                    "Content-Type: text/plain\n\n"\
                    "Hello tupaki!";
                m_socketListen->send( resp );
            } break;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerService::onDone( uint32_t )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerService::setScene( const Mengine::ScenePtr & _scene )
    {
        m_scene = _scene;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerService::update()
    {
        if( m_socketListen == nullptr )
        {
            m_socketListen = SOCKET_SYSTEM()->createSocket();

            SocketConnectInfo sci = { "0.0.0.0", "21038" };
            m_socketListen->bind( sci );

            m_threadJobListenToSocket = THREAD_SERVICE()->createJob( 100u );

            THREAD_SERVICE()->createThread( STRINGIZE_STRING_LOCAL( "NodeDebuggerListenThread" ), -1, __FILE__, __LINE__ );

            THREAD_SERVICE()->addTask( STRINGIZE_STRING_LOCAL( "NodeDebuggerListenThread" ), m_threadJobListenToSocket );

            m_serverState = NodeDebuggerServerState::WaitingForClient;

            m_threadJobListenToSocket->addWorker( this );
        }
    }
}