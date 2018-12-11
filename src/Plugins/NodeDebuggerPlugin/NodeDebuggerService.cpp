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

#define NODE_SERIALIZATION_INGAME
#include "NodePropsSerializaton.h"


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

                APPLICATION_SERVICE()->setNopause( true );

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
                if( m_receivedData.size() > sizeof( PacketHeader ) )
                {
                    ThreadMutexScope mutexLock( m_dataMutex );

                    // check if we have enough data to form a packet
                    PacketHeader* hdr = reinterpret_cast<PacketHeader *>( m_receivedData.data() );
                    while(hdr != nullptr && hdr->payloadSize <= ( m_receivedData.size() - sizeof( uint32_t ) ) )
                    {
                        // received garbage - nothing fancy, just disconnect
                        if( hdr->magic != PACKET_MAGIC )
                        {
                            //DisconnectFromServer();
                            return true;
                        }

                        const size_t dataSizeWithHeader = hdr->payloadSize + sizeof( PacketHeader );

                        NodeDebuggerPacket packet;
                        packet.payload.resize( hdr->payloadSize );
                        memcpy( packet.payload.data(), m_receivedData.data() + sizeof( PacketHeader ), hdr->payloadSize );
                        m_incomingPackets.emplace_back( packet );

                        // now remove this packet data from the buffer
                        const size_t newSize = m_receivedData.size() - dataSizeWithHeader;
                        if( newSize )
                        {
                            memmove( m_receivedData.data(), m_receivedData.data() + dataSizeWithHeader, newSize );
                            m_receivedData.resize( newSize );

                            hdr = reinterpret_cast<PacketHeader *>( m_receivedData.data() );
                        }
                        else
                        {
                            m_receivedData.clear();
                            hdr = nullptr;
                        }

                        if( newSize <= sizeof( PacketHeader ) )
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
            PacketHeader hdr;
            hdr.magic = PACKET_MAGIC;
            hdr.payloadSize = static_cast<uint32_t>( _packet.payload.size() );

            InsertPacketHeader( _packet.payload, hdr );

            ThreadMutexScope mutexLock( m_dataMutex );
            m_outgoingPackets.emplace_back( _packet );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerService::serializeRender( const RenderInterface * _render, pugi::xml_node & _xmlParentNode )
    {
        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Render" );

        serializeNodeProp( _render->isRenderEnable(), "enable", xmlNode );
        serializeNodeProp( _render->isHide(), "hide", xmlNode );
        serializeNodeProp( _render->getLocalColor(), "color", xmlNode );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerService::serializeAnimation( const AnimationInterface * _animation, pugi::xml_node & _xmlParentNode )
    {
        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Animation" );

        serializeNodeProp( _animation->isLoop(), "loop", xmlNode );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerService::serializeNode( const Mengine::NodePtr & _node, pugi::xml_node & _xmlParentNode )
    {
        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Node" );

        xmlNode.append_attribute( "uid" ).set_value( _node->getUniqueIdentity() );
        xmlNode.append_attribute( "name" ).set_value( _node->getName().c_str() );
        xmlNode.append_attribute( "type" ).set_value( _node->getType().c_str() );

        serializeNodeProp( _node->isEnable(), "enable", xmlNode );
        serializeNodeProp( _node->getScale(), "scale", xmlNode );
        serializeNodeProp( _node->getSkew(), "skew", xmlNode );

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

            _node->foreachChildren( [this, &xmlChildrenContainer]( const NodePtr & _child )
            {
                this->serializeNode( _child, xmlChildrenContainer );
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

#ifdef _DEBUG
        const unsigned int xmlFlags = pugi::format_indent;
#else
        const unsigned int xmlFlags = pugi::format_raw;
#endif
        doc.save( writer, "  ", xmlFlags, pugi::encoding_utf8 );

        sendPacket( packet );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerService::processPacket( NodeDebuggerPacket & _packet )
    {
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_buffer( _packet.payload.data(), _packet.payload.size() );
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
            pugi::xml_node xmlNode = payloadNode.child( "Node" );

            if( xmlNode )
            {
                receiveChangedNode( xmlNode );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerService::receiveChangedNode( const pugi::xml_node& _xmlNode )
    {
        String pathStr = _xmlNode.attribute( "path" ).value();
        VectorNodePath path = stringToPath( pathStr );

        NodePtr node = Mengine::findUniqueNode( m_scene, path );

        if( node != nullptr )
        {
            deserializeNodeProp<bool>( node, "enable", _xmlNode, []( auto _node, auto _value )
            {
                if( _value != _node->isEnable() )
                {
                    if( _value )
                    {
                        _node->enable();
                    }
                    else
                    {
                        _node->disable();
                    }
                }
            } );

            deserializeNodeProp<mt::vec3f>( node, "scale", _xmlNode, []( auto _node, auto _value )
            {
                _node->setScale( _value );
            } );

            deserializeNodeProp<mt::vec2f>( node, "skew", _xmlNode, []( auto _node, auto _value )
            {
                _node->setSkew( _value );
            } );

            pugi::xml_node renderNode = _xmlNode.child( "Render" );
            pugi::xml_node animationNode = _xmlNode.child( "Animation" );

            RenderInterface * render = node->getRender();
            if( render && renderNode )
            {
                deserializeNodeProp<bool>( node, "enable", renderNode, []( auto _node, auto _value )
                {
                    if( _node->getRender()->isRenderEnable() != _value )
                    {
                        _node->getRender()->setRenderEnable( _value );
                    }
                } );

                deserializeNodeProp<bool>( node, "hide", renderNode, []( auto _node, auto _value )
                {
                    if( _node->getRender()->isHide() != _value )
                    {
                        _node->getRender()->setHide( _value );
                    }
                } );

                deserializeNodeProp<Mengine::Color>( node, "color", renderNode, []( auto _node, auto _value )
                {
                    _node->getRender()->setLocalColor( _value );
                } );
            }

            AnimationInterface * animation = node->getAnimation();
            if( animation && animationNode )
            {
                deserializeNodeProp<bool>( node, "loop", animationNode, []( auto _node, auto _value )
                {
                    if( _node->getAnimation()->isLoop() != _value )
                    {
                        _node->getAnimation()->setLoop( _value );
                    }
                } );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    VectorNodePath NodeDebuggerService::stringToPath( const String & _str )
    {
        VectorNodePath path;

        if( !_str.empty() )
        {
            const char* ptr = _str.c_str();
            uint32_t uid = 0;
            while( *ptr )
            {
                if( *ptr == '/' )
                {
                    path.push_back( uid );
                    uid = 0;
                }
                else
                {
                    uid *= 10;
                    uid += static_cast<uint32_t>( *ptr - '0' );
                }

                ++ptr;
            }
        }

        return path;
    }
    //////////////////////////////////////////////////////////////////////////
}