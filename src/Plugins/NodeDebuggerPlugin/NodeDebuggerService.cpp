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

#include "Plugins/XmlCodecPlugin/XmlCodecInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/ThreadTask.h"
#include "Kernel/SchedulerHelper.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/Scene.h"

#include "Config/Stringstream.h"

#include "pybind/pybind.hpp"
#include "stdex/allocator_report.h"

#define PUGIXML_NO_STL
#define PUGIXML_HEADER_ONLY
#include "pugixml.hpp"


#include <iomanip>


class MyXMLWriter : public pugi::xml_writer
{
public:
    MyXMLWriter() = delete;
    MyXMLWriter( const MyXMLWriter& ) = delete;
    ~MyXMLWriter() = default;

    MyXMLWriter( Mengine::Vector<uint8_t>& buffer )
        : m_buffer( buffer )
    {
    }

    virtual void write( const void* data, size_t size ) override
    {
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>( data );
        m_buffer.insert( m_buffer.end(), ptr, ptr + size );
    }

private:
    Mengine::Vector<uint8_t>& m_buffer;
};


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
        if( m_socket == nullptr )
        {
            return false;
        }

        switch( m_serverState )
        {
            case NodeDebuggerServerState::WaitingForClient:
            {
                if( !m_socket->waitForClient() )
                {
                    m_serverState = NodeDebuggerServerState::Invalid;
                    return false;
                }

                m_serverState = NodeDebuggerServerState::Connected;

                sendScene( m_scene );
            } break;

            case NodeDebuggerServerState::Connected:
            {
                // check if need to send data
                m_dataMutex->lock();
                {
                    if( !m_outgoingPackets.empty() )
                    {
                        for( const auto& p : m_outgoingPackets)
                        {
                            m_socket->send( p.payload.data(), p.payload.size() );
                        }

                        m_outgoingPackets.clear();
                    }
                }
                m_dataMutex->unlock();

                // now check if we received something (100 ms)
                const bool haveSomeData = m_socket->waitForData( 100u );
                if( haveSomeData )
                {
                    ThreadMutexScope mutexLock( m_dataMutex );

                    static const size_t BUFFER_SIZE = 4096;
                    uint8_t buffer[BUFFER_SIZE];

                    size_t bytesReceived = 0;
                    do
                    {
                        bytesReceived = m_socket->recieve( buffer, BUFFER_SIZE );

                        if( bytesReceived )
                        {
                            m_receivedData.insert( m_receivedData.end(), &buffer[0], &buffer[bytesReceived] );
                        }
                    }
                    while( bytesReceived == BUFFER_SIZE );
                }

                // check if we have read something
                if( m_receivedData.size() > sizeof( uint32_t ) )
                {
                    // check if we have enough data to form a packet
                    uint32_t dataSize = *reinterpret_cast<uint32_t*>( m_receivedData.data() );
                    while( dataSize <= ( m_receivedData.size() - sizeof( uint32_t ) ) )
                    {
                        ThreadMutexScope mutexLock( m_dataMutex );

                        const size_t dataSizeWithSize = dataSize + sizeof( uint32_t );

                        NodeDebuggerPacket packet;
                        packet.payload.resize( dataSize );
                        memcpy( packet.payload.data(), m_receivedData.data() + sizeof( uint32_t ), dataSize );
                        m_incomingPackets.emplace_back( packet );

                        // now remove this packet data from the buffer
                        const size_t newSize = m_receivedData.size() - dataSizeWithSize;
                        if( newSize )
                        {
                            memmove( m_receivedData.data(), m_receivedData.data() + dataSizeWithSize, newSize );
                            m_receivedData.resize( newSize );

                            dataSize = *reinterpret_cast<uint32_t*>( m_receivedData.data() );
                        }
                        else
                        {
                            m_receivedData.clear();
                            dataSize = 0;
                        }

                        if( newSize <= sizeof( uint32_t ) )
                        {
                            break;
                        }
                    }
                }
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
        if( m_scene != _scene )
        {
            m_scene = _scene;

            if( m_serverState == NodeDebuggerServerState::Connected )
            {
                sendScene( m_scene );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerService::update()
    {
        if( m_socket == nullptr )
        {
            privateInit();
        }
        else
        {
            ThreadMutexScope mutexLock( m_dataMutex );
            if( !m_incomingPackets.empty() )
            {
                processPacket( m_incomingPackets.front() );
                m_incomingPackets.pop_front();
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerService::privateInit()
    {
        m_socket = SOCKET_SYSTEM()->createSocket();

        SocketConnectInfo sci = { "0.0.0.0", "18790" };
        m_socket->bind(sci);

        m_threadJob = THREAD_SERVICE()->createJob( 50u );

        THREAD_SERVICE()->createThread(STRINGIZE_STRING_LOCAL( "NodeDebuggerListenThread"), -1, __FILE__, __LINE__ );

        THREAD_SERVICE()->addTask(STRINGIZE_STRING_LOCAL( "NodeDebuggerListenThread" ), m_threadJob);

        m_dataMutex = THREAD_SERVICE()->createMutex( __FILE__, __LINE__ );

        m_serverState = NodeDebuggerServerState::WaitingForClient;

        m_threadJob->addWorker( this );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerService::sendPacket( NodeDebuggerPacket & _packet )
    {
        if( !_packet.payload.empty() )
        {
            const uint32_t payloadLength = static_cast<uint32_t>( _packet.payload.size() );
            const uint8_t* begin = reinterpret_cast<const uint8_t*>( &payloadLength );
            const uint8_t* end = begin + sizeof( payloadLength );
            _packet.payload.insert( _packet.payload.begin(), begin, end );

            ThreadMutexScope mutexLock( m_dataMutex );
            m_outgoingPackets.emplace_back( _packet );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void serializeRender( RenderInterface * _render, pugi::xml_node & _xmlParentNode )
    {
        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Render" );

        xmlNode.append_attribute( "enable" ).set_value( _render->isRenderEnable() );
        xmlNode.append_attribute( "hide" ).set_value( _render->isHide() );
    }
    //////////////////////////////////////////////////////////////////////////
    void serializeAnimation( AnimationInterface * _animation, pugi::xml_node & _xmlParentNode )
    {
        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Animation" );

        xmlNode.append_attribute( "loop" ).set_value( _animation->isLoop() );
    }
    //////////////////////////////////////////////////////////////////////////
    void serializeNode( const Mengine::NodePtr & _node, pugi::xml_node & _xmlParentNode )
    {
        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Node" );

        xmlNode.append_attribute( "name" ).set_value( _node->getName().c_str() );
        xmlNode.append_attribute( "type" ).set_value( _node->getType().c_str() );
        xmlNode.append_attribute( "active" ).set_value( _node->isActivate() );

        RenderInterface * render = _node->getRender();
        if( render )
        {
            serializeRender( render, xmlNode );
        }

        AnimationInterface * animation = _node->getAnimation();
        if( animation )
        {
            serializeAnimation( animation, xmlNode );
        }

        if( !_node->emptyChildren() )
        {
            pugi::xml_node xmlChildrenContainer = xmlNode.append_child( "Children" );

            _node->foreachChildrenUnslug( [&xmlChildrenContainer]( const NodePtr & _child )
            {
                serializeNode( _child, xmlChildrenContainer );
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerService::sendScene( const Mengine::ScenePtr & _scene )
    {
        pugi::xml_document doc;

        pugi::xml_node packetNode = doc.append_child( "Packet" );
        packetNode.append_attribute( "type" ).set_value( "Scene" );

        pugi::xml_node payloadNode = packetNode.append_child( "Payload" );

        if( _scene != nullptr )
        {
            serializeNode( _scene, payloadNode );
        }

        NodeDebuggerPacket packet;

        MyXMLWriter writer( packet.payload );

        doc.save( writer, "  ", pugi::format_indent/*pugi::format_raw*/, pugi::encoding_utf8 );

        sendPacket( packet );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerService::processPacket( NodeDebuggerPacket & _packet )
    {
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_buffer(_packet.payload.data(), _packet.payload.size() );
        if( !result )
        {
            return;
        }

        pugi::xml_node packetNode = doc.child( "Packet" );
        if( !packetNode )
        {
            return;
        }

        pugi::xml_attribute typeAttrib = packetNode.attribute( "type" );
        if( !typeAttrib )
        {
            return;
        }

        // type name to enum
        std::string typeStr = typeAttrib.value();

        pugi::xml_node payloadNode = packetNode.child( "Payload" );
        if( !payloadNode )
        {
            return;
        }

        if( typeStr == "Node" )
        {
            //ReceiveScene( payloadNode );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}