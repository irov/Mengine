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
#include "Interface/VocabularyServiceInterface.h"

#include "NodeDebuggerSerialization.h"

#include "ShapeDebuggerBoundingBox.h"
#include "HotSpotPolygonDebuggerBoundingBox.h"

#include "Kernel/Assertion.h"
#include "Kernel/ThreadTask.h"
#include "Kernel/SchedulerHelper.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/AssertionVocabulary.h"
#include "Kernel/Scene.h"

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
        , m_shouldRecreateServer( false )
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
        VOCALUBARY_SET( NodeDebuggerBoundingBoxInterface, STRINGIZE_STRING_LOCAL( "NodeDebuggerBoundingBox" ), STRINGIZE_STRING_LOCAL( "ShapeQuadFixed" ), new FactorableUnique<ShapeDebuggerBoundingBox>() );
        VOCALUBARY_SET( NodeDebuggerBoundingBoxInterface, STRINGIZE_STRING_LOCAL( "NodeDebuggerBoundingBox" ), STRINGIZE_STRING_LOCAL( "ShapeQuadFlex" ), new FactorableUnique<ShapeDebuggerBoundingBox>() );
        VOCALUBARY_SET( NodeDebuggerBoundingBoxInterface, STRINGIZE_STRING_LOCAL( "NodeDebuggerBoundingBox" ), STRINGIZE_STRING_LOCAL( "HotSpotPolygon" ), new FactorableUnique<HotSpotPolygonDebuggerBoundingBox>() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerService::_finalizeService()
    {
        VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "NodeDebuggerBoundingBox" ), STRINGIZE_STRING_LOCAL( "ShapeQuadFixed" ) );
        VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "NodeDebuggerBoundingBox" ), STRINGIZE_STRING_LOCAL( "ShapeQuadFlex" ) );
        VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "NodeDebuggerBoundingBox" ), STRINGIZE_STRING_LOCAL( "HotSpotPolygon" ) );

        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "NodeDebuggerBoundingBox" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeDebuggerService::onWork( uint32_t )
    {
        switch( m_serverState )
        {
        case NodeDebuggerServerState::WaitingForClient:
            {
                const int check = m_socket->checkForClientConnection();
                if( check < 0 )
                {
                    // failed
                    m_serverState = NodeDebuggerServerState::Invalid;
                    return false;
                }
                else if( check > 0 )
                {
                    // got client connection
                    m_serverState = NodeDebuggerServerState::Connected;

                    APPLICATION_SERVICE()->setNopause( true );

                    sendScene( m_scene );
                }
            } break;

        case NodeDebuggerServerState::Connected:
            {
                // check if need to send data
                m_dataMutex->lock();
                {
                    if( !m_outgoingPackets.empty() )
                    {
                        for( const auto& p : m_outgoingPackets )
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

                    int bytesReceived = 0;
                    bool clientDisconnected = false;
                    do
                    {
                        bytesReceived = m_socket->receive( buffer, BUFFER_SIZE );

                        if( bytesReceived > 0 )
                        {
                            m_receivedData.insert( m_receivedData.end(), &buffer[0], &buffer[bytesReceived] );
                        }
                        else
                        {
                            clientDisconnected = true;
                        }
                    } while( !clientDisconnected && bytesReceived == static_cast<int>(BUFFER_SIZE) );

                    if( clientDisconnected )
                    {
                        m_shouldRecreateServer = true;
                        m_serverState = NodeDebuggerServerState::Invalid;
                        return true;
                    }
                }

                // check if we have read something
                if( m_receivedData.size() > sizeof( PacketHeader ) )
                {
                    ThreadMutexScope mutexLock( m_dataMutex );

                    // check if we have enough data to form a packet
                    PacketHeader* hdr = reinterpret_cast<PacketHeader *>(m_receivedData.data());
                    while( hdr != nullptr && hdr->payloadSize <= (m_receivedData.size() - sizeof( uint32_t )) )
                    {
                        // received garbage - nothing fancy, just disconnect
                        if( hdr->magic != PACKET_MAGIC )
                        {
                            m_shouldRecreateServer = true;
                            m_serverState = NodeDebuggerServerState::Invalid;
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

                            hdr = reinterpret_cast<PacketHeader *>(m_receivedData.data());
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
    void NodeDebuggerService::setScene( const ScenePtr & _scene )
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
        if( m_shouldRecreateServer )
        {
            recreateServer();
        }

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
    static bool s_absorbBoundingBox( const NodePtr & _node, mt::box2f & _bb )
    {
        bool successul = false;

        mt::box2f absorb_bb;
        mt::insideout_box( absorb_bb );

        const ConstString & type = _node->getType();

        NodeDebuggerBoundingBoxInterfacePtr boundingBox = VOCALUBARY_GET( STRINGIZE_STRING_LOCAL( "NodeDebuggerBoundingBox" ), type );

        if( boundingBox != nullptr )
        {
            mt::box2f bbox;
            if( boundingBox->getBoundingBox( _node, &bbox ) == true )
            {
                mt::merge_box( absorb_bb, bbox );

                successul = true;
            }
        }

        _node->foreachChildren( [&absorb_bb, &successul]( const NodePtr & _child )
        {
            mt::box2f child_bb;
            if( s_absorbBoundingBox( _child, child_bb ) == true )
            {
                successul = true;
            }

            mt::merge_box( absorb_bb, child_bb );
        } );

        _bb = absorb_bb;

        return successul;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerService::render( const RenderContext * _context )
    {
        if( m_selectedNodePath.empty() == true )
        {
            return;
        }

        NodePtr node = Helper::findUniqueNode( m_scene, m_selectedNodePath );

        if( node == nullptr )
        {
            return;
        }

        mt::box2f bbox;
        if( s_absorbBoundingBox( node, bbox ) == false )
        {
            return;
        }

        const RenderMaterialInterfacePtr & debugMaterial = RENDERMATERIAL_SERVICE()
            ->getDebugMaterial();

        RenderVertex2D * vertices = RENDER_SERVICE()
            ->getDebugRenderVertex2D( 5 );

        vertices[0].position = mt::vec3f( bbox.minimum.x, bbox.minimum.y, 0.f );
        vertices[1].position = mt::vec3f( bbox.maximum.x, bbox.minimum.y, 0.f );
        vertices[2].position = mt::vec3f( bbox.maximum.x, bbox.maximum.y, 0.f );
        vertices[3].position = mt::vec3f( bbox.minimum.x, bbox.maximum.y, 0.f );
        vertices[4].position = vertices[0].position;

        vertices[0].color = Helper::makeARGB( 1.f, 0.f, 1.f, 1.f );
        vertices[1].color = Helper::makeARGB( 1.f, 0.f, 1.f, 1.f );
        vertices[2].color = Helper::makeARGB( 1.f, 0.f, 1.f, 1.f );
        vertices[3].color = Helper::makeARGB( 1.f, 0.f, 1.f, 1.f );
        vertices[4].color = Helper::makeARGB( 1.f, 0.f, 1.f, 1.f );

        for( size_t i = 0; i < 4; ++i )
        {
            RENDER_SERVICE()
                ->addRenderLine( _context
                    , debugMaterial
                    , &vertices[i]
                    , 2
                    , nullptr
                    , false );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerService::privateInit()
    {
        m_shouldRecreateServer = true;

        m_threadJob = THREAD_SERVICE()
            ->createJob( 50u );

        THREAD_SERVICE()
            ->createThread( STRINGIZE_STRING_LOCAL( "NodeDebuggerListenThread" ), -1, __FILE__, __LINE__ );

        THREAD_SERVICE()
            ->addTask( STRINGIZE_STRING_LOCAL( "NodeDebuggerListenThread" ), m_threadJob );

        m_dataMutex = THREAD_SERVICE()
            ->createMutex( __FILE__, __LINE__ );

        m_threadJob->addWorker( this );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerService::recreateServer()
    {
        m_socket = SOCKET_SYSTEM()->createSocket();

        SocketConnectInfo sci = { "0.0.0.0", "18790" };
        m_socket->bind( sci, false );

        m_serverState = NodeDebuggerServerState::WaitingForClient;

        m_receivedData.resize( 0 );
        m_incomingPackets.resize( 0 );
        m_outgoingPackets.resize( 0 );

        m_shouldRecreateServer = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerService::sendPacket( NodeDebuggerPacket & _packet )
    {
        if( !_packet.payload.empty() )
        {
            PacketHeader hdr;
            hdr.magic = PACKET_MAGIC;
            hdr.payloadSize = static_cast<uint32_t>(_packet.payload.size());

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
    void NodeDebuggerService::serializeNode( const NodePtr & _node, pugi::xml_node & _xmlParentNode )
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
    void NodeDebuggerService::sendScene( const ScenePtr & _scene )
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
        const uint32_t xmlFlags = pugi::format_indent;
#else
        const uint32_t xmlFlags = pugi::format_raw;
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
        else if( typeStr == "Selection" )
        {
            m_selectedNodePath.clear();

            pugi::xml_node xmlNode = payloadNode.child( "Path" );

            if( xmlNode )
            {
                pugi::xml_attribute valueAttr = xmlNode.attribute( "value" );

                if( valueAttr )
                {
                    m_selectedNodePath = stringToPath( valueAttr.value() );
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerService::receiveChangedNode( const pugi::xml_node & _xmlNode )
    {
        String pathStr = _xmlNode.attribute( "path" ).value();
        VectorNodePath path = stringToPath( pathStr );

        NodePtr node = Helper::findUniqueNode( m_scene, path );

        if( node != nullptr )
        {
            deserializeNodeProp<bool>( "enable", _xmlNode, [node]( auto _value )
            {
                if( _value != node->isEnable() )
                {
                    if( _value )
                    {
                        node->enable();
                    }
                    else
                    {
                        node->disable();
                    }
                }
            } );

            deserializeNodeProp<mt::vec3f>( "scale", _xmlNode, [node]( auto _value )
            {
                node->setScale( _value );
            } );

            deserializeNodeProp<mt::vec2f>( "skew", _xmlNode, [node]( auto _value )
            {
                node->setSkew( _value );
            } );

            pugi::xml_node renderNode = _xmlNode.child( "Render" );
            pugi::xml_node animationNode = _xmlNode.child( "Animation" );

            RenderInterface * render = node->getRender();
            if( render && renderNode )
            {
                deserializeNodeProp<bool>( "enable", renderNode, [render]( auto _value )
                {
                    if( render->isRenderEnable() != _value )
                    {
                        render->setRenderEnable( _value );
                    }
                } );

                deserializeNodeProp<bool>( "hide", renderNode, [render]( auto _value )
                {
                    if( render->isHide() != _value )
                    {
                        render->setHide( _value );
                    }
                } );

                deserializeNodeProp<Color>( "color", renderNode, [render]( auto _value )
                {
                    render->setLocalColor( _value );
                } );
            }

            AnimationInterface * animation = node->getAnimation();
            if( animation && animationNode )
            {
                deserializeNodeProp<bool>( "loop", animationNode, [animation]( auto _value )
                {
                    if( animation->isLoop() != _value )
                    {
                        animation->setLoop( _value );
                    }
                } );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    VectorNodePath NodeDebuggerService::stringToPath( const String & _str )
    {
        VectorNodePath path;

        if( !_str.empty() && _str[0] != '-' )
        {
            const Char* ptr = _str.c_str();

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
                    uid += static_cast<uint32_t>(*ptr - '0');
                }

                ++ptr;
            }
        }

        return path;
    }
    //////////////////////////////////////////////////////////////////////////
}