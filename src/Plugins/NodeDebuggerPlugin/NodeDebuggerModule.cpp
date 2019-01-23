#include "NodeDebuggerModule.h"

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
#include "Interface/TextServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"

#include "NodeDebuggerSerialization.h"

#include "HotSpotPolygonDebuggerBoundingBox.h"
#include "TextFieldDebuggerBoundingBox.h"

#include "Kernel/Assertion.h"
#include "Kernel/ThreadTask.h"
#include "Kernel/SchedulerHelper.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/AssertionVocabulary.h"
#include "Kernel/NodeRenderHelper.h"
#include "Kernel/Scene.h"
#include "Kernel/StringFormat.h"
#include "Kernel/Logger.h"

#include "Config/Stringstream.h"

#include "pybind/pybind.hpp"
#include "stdex/allocator_report.h"

#include <iomanip>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    NodeDebuggerModule::NodeDebuggerModule()
        : m_serverState( NodeDebuggerServerState::Invalid )
        , m_shouldRecreateServer( false )
        , m_shouldUpdateScene( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    NodeDebuggerModule::~NodeDebuggerModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeDebuggerModule::_initializeModule()
    {
        VOCALUBARY_SET( NodeDebuggerBoundingBoxInterface, STRINGIZE_STRING_LOCAL( "NodeDebuggerBoundingBox" ), STRINGIZE_STRING_LOCAL( "HotSpotPolygon" ), new FactorableUnique<HotSpotPolygonDebuggerBoundingBox>() );
        VOCALUBARY_SET( NodeDebuggerBoundingBoxInterface, STRINGIZE_STRING_LOCAL( "NodeDebuggerBoundingBox" ), STRINGIZE_STRING_LOCAL( "TextField" ), new FactorableUnique<TextFieldDebuggerBoundingBox>() );

        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_CHANGE_SCENE_COMPLETE, this, &NodeDebuggerModule::notifyChangeScene );
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_REMOVE_SCENE_DESTROY, this, &NodeDebuggerModule::notifyRemoveSceneDestroy );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::_finalizeModule()
    {
        VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "NodeDebuggerBoundingBox" ), STRINGIZE_STRING_LOCAL( "HotSpotPolygon" ) );
        VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "NodeDebuggerBoundingBox" ), STRINGIZE_STRING_LOCAL( "TextField" ) );

        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "NodeDebuggerBoundingBox" ) );

        NOTIFICATION_REMOVEOBSERVER( NOTIFICATOR_CHANGE_SCENE_COMPLETE, this );
        NOTIFICATION_REMOVEOBSERVER( NOTIFICATOR_REMOVE_SCENE_DESTROY, this );

    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeDebuggerModule::onWork( uint32_t )
    {
        switch( m_serverState )
        {
        case NodeDebuggerServerState::Invalid:
            {
            }break;
        case NodeDebuggerServerState::WaitingForClient:
            {
                const int32_t check = m_socket->checkForClientConnection();
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

                    APPLICATION_SERVICE()
                        ->setNopause( true );

                    m_shouldUpdateScene = true;
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

                    int32_t bytesReceived = 0;
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
                    } while( !clientDisconnected && bytesReceived == static_cast<int32_t>(BUFFER_SIZE) );

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
    void NodeDebuggerModule::onDone( uint32_t )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::setScene( const ScenePtr & _scene )
    {
        if( m_scene != _scene )
        {
            m_scene = _scene;

            m_shouldUpdateScene = true;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::updateScene()
    {
        m_shouldUpdateScene = false;

        if( m_serverState == NodeDebuggerServerState::Connected )
        {
            this->sendScene( m_scene );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::_update( bool _focus )
    {
        MENGINE_UNUSED( _focus );

        if( m_shouldRecreateServer )
        {
            this->recreateServer();
        }

        if( m_shouldUpdateScene == true )
        {
            this->updateScene();
        }

        if( m_socket == nullptr )
        {
            this->privateInit();
        }
        else
        {
            ThreadMutexScope mutexLock( m_dataMutex );
            if( !m_incomingPackets.empty() )
            {
                this->processPacket( m_incomingPackets.front() );
                m_incomingPackets.pop_front();
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    static bool s_absorbBoundingBox( const NodePtr & _node, mt::box2f & _bb )
    {
        if( _node->isEnable() == false )
        {
            return false;
        }

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
        else
        {
            RenderInterface * render = _node->getRender();

            if( render != nullptr )
            {
                const mt::box2f * rbbox = render->getBoundingBox();

                if( rbbox != nullptr )
                {
                    mt::merge_box( absorb_bb, *rbbox );

                    successul = true;
                }
            }
        }

        _node->foreachChildren( [&absorb_bb, &successul]( const NodePtr & _child )
        {
            mt::box2f child_bb;
            if( s_absorbBoundingBox( _child, child_bb ) == true )
            {
                mt::merge_box( absorb_bb, child_bb );

                successul = true;
            }            
        } );

        _bb = absorb_bb;

        return successul;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::_render( const RenderContext * _context )
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

        RenderContext node_context = *_context;
        Helper::getNodeRenderContext( node, &node_context );

        mt::box2f bbox;
        if( s_absorbBoundingBox( node, bbox ) == false )
        {
            return;
        }

        const RenderCameraInterfacePtr & camera = node_context.camera;

        const mt::mat4f & vpminv = camera->getCameraViewProjectionMatrixInv();

        mt::box2f bcrop;
        mt::mul_v2_v2_m4( bcrop.minimum, mt::vec2f( -1.f, 1.f ), vpminv );
        mt::mul_v2_v2_m4( bcrop.maximum, mt::vec2f( 1.f, -1.f ), vpminv );
        
        mt::crop_box( bbox, bcrop );

        const RenderMaterialInterfacePtr & debugMaterial = RENDERMATERIAL_SERVICE()
            ->getDebugTriangleMaterial();

        RenderVertex2D * vertices = RENDER_SERVICE()
            ->getDebugRenderVertex2D( 8 );

        float offset = 2.0f;

        vertices[0].position = mt::vec3f( bbox.minimum.x - offset, bbox.minimum.y - offset, 0.f );
        vertices[1].position = mt::vec3f( bbox.maximum.x + offset, bbox.minimum.y - offset, 0.f );
        vertices[2].position = mt::vec3f( bbox.maximum.x + offset, bbox.maximum.y + offset, 0.f );
        vertices[3].position = mt::vec3f( bbox.minimum.x - offset, bbox.maximum.y + offset, 0.f );
        vertices[4].position = mt::vec3f( bbox.minimum.x + offset, bbox.minimum.y + offset, 0.f );
        vertices[5].position = mt::vec3f( bbox.maximum.x - offset, bbox.minimum.y + offset, 0.f );
        vertices[6].position = mt::vec3f( bbox.maximum.x - offset, bbox.maximum.y - offset, 0.f );
        vertices[7].position = mt::vec3f( bbox.minimum.x + offset, bbox.maximum.y - offset, 0.f );

        ColorValue_ARGB color = Helper::makeARGB( 1.f, 0.f, 1.f, 1.f );

        for( uint32_t index = 0; index != 8; ++index )
        {
            vertices[index].color = color;
        }

        RenderIndex * indices = RENDER_SERVICE()
            ->getDebugRenderIndex( 2 * 3 * 4 );

        indices[0 * 6 + 0 * 3 + 0] = 0;
        indices[0 * 6 + 0 * 3 + 1] = 1;
        indices[0 * 6 + 0 * 3 + 2] = 4;
        indices[0 * 6 + 1 * 3 + 0] = 4;
        indices[0 * 6 + 1 * 3 + 1] = 1;
        indices[0 * 6 + 1 * 3 + 2] = 5;

        indices[1 * 6 + 0 * 3 + 0] = 1;
        indices[1 * 6 + 0 * 3 + 1] = 2;
        indices[1 * 6 + 0 * 3 + 2] = 5;
        indices[1 * 6 + 1 * 3 + 0] = 5;
        indices[1 * 6 + 1 * 3 + 1] = 2;
        indices[1 * 6 + 1 * 3 + 2] = 6;

        indices[2 * 6 + 0 * 3 + 0] = 2;
        indices[2 * 6 + 0 * 3 + 1] = 3;
        indices[2 * 6 + 0 * 3 + 2] = 6;
        indices[2 * 6 + 1 * 3 + 0] = 6;
        indices[2 * 6 + 1 * 3 + 1] = 3;
        indices[2 * 6 + 1 * 3 + 2] = 7;

        indices[3 * 6 + 0 * 3 + 0] = 3;
        indices[3 * 6 + 0 * 3 + 1] = 0;
        indices[3 * 6 + 0 * 3 + 2] = 7;
        indices[3 * 6 + 1 * 3 + 0] = 7;
        indices[3 * 6 + 1 * 3 + 1] = 0;
        indices[3 * 6 + 1 * 3 + 2] = 4;

        RENDER_SERVICE()
            ->addRenderObject( &node_context
                , debugMaterial
                , nullptr
                , vertices, 8
                , indices, 24
                , &bbox
                , false );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::privateInit()
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
    void NodeDebuggerModule::recreateServer()
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
    void NodeDebuggerModule::sendPacket( NodeDebuggerPacket & _packet )
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
    void NodeDebuggerModule::serializeTransformation( const TransformationPtr & _transformation, pugi::xml_node & _xmlParentNode )
    {
        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Transformation" );

        serializeNodeProp( _transformation->getLocalPosition(), "position", xmlNode );
        serializeNodeProp( _transformation->getOrigin(), "origin", xmlNode );
        serializeNodeProp( _transformation->getSkew(), "skew", xmlNode );
        serializeNodeProp( _transformation->getScale(), "scale", xmlNode );
        serializeNodeProp( _transformation->getOrientation(), "orientation", xmlNode );
        serializeNodeProp( _transformation->getWorldPosition(), "worldPosition", xmlNode );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::serializeRender( const RenderInterface * _render, pugi::xml_node & _xmlParentNode )
    {
        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Render" );

        serializeNodeProp( _render->isRenderEnable(), "rendering", xmlNode );
        serializeNodeProp( _render->isHide(), "hide", xmlNode );
        serializeNodeProp( _render->getLocalColor(), "color", xmlNode );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::serializeAnimation( const AnimationInterface * _animation, pugi::xml_node & _xmlParentNode )
    {
        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Animation" );

        serializeNodeProp( _animation->isLoop(), "loop", xmlNode );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::serializeTextField( const TextFieldPtr & _textField, pugi::xml_node & _xmlParentNode )
    {
        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Type:TextField" );

        serializeNodeProp( _textField->getMaxLength(), "MaxLength", xmlNode );
        serializeNodeProp( _textField->getWrap(), "Wrap", xmlNode );

        const ConstString & textID = _textField->getTextID();
        const ConstString & textAliasEnvironment = _textField->getTextAliasEnvironment();

        serializeNodeProp( textID, "TextID", xmlNode );
        serializeNodeProp( textAliasEnvironment, "TextAliasEnvironment", xmlNode );

        const ConstString & aliasTestId = TEXT_SERVICE()
            ->getTextAlias( textAliasEnvironment, textID );

        TextEntryInterfacePtr textEntry;
        if( TEXT_SERVICE()
            ->existText( aliasTestId, &textEntry ) == false )
        {
            serializeNodeProp( false, "HasText", xmlNode );
        }
        else
        {
            serializeNodeProp( true, "HasText", xmlNode );

            const String & textValue = textEntry->getValue();

            serializeNodeProp( textValue, "Format", xmlNode );

            VectorString textFormatArgs = _textField->getTextFormatArgs();

            TEXT_SERVICE()
                ->getTextAliasArguments( textAliasEnvironment, textID, textFormatArgs );

            String fmt;
            Helper::getStringFormat( fmt, textValue, textFormatArgs );

            serializeNodeProp( fmt, "Text", xmlNode );
        }

        serializeNodeProp( _textField->getFontName(), "FontName", xmlNode );
        serializeNodeProp( _textField->getFontColor(), "FontColor", xmlNode );
        serializeNodeProp( _textField->getLineOffset(), "LineOffset", xmlNode );
        serializeNodeProp( _textField->getCharOffset(), "CharOffset", xmlNode );
        serializeNodeProp( _textField->getCharScale(), "CharScale", xmlNode );
        serializeNodeProp( (uint32_t)_textField->getHorizontAlign(), "HorizontAlign", xmlNode );
        serializeNodeProp( (uint32_t)_textField->getVerticalAlign(), "VerticalAlign", xmlNode );

        serializeNodeProp( _textField->calcFontName(), "TotalFontName", xmlNode );
        serializeNodeProp( _textField->calcFontColor(), "TotalFontColor", xmlNode );
        serializeNodeProp( _textField->calcLineOffset(), "TotalLineOffset", xmlNode );
        serializeNodeProp( _textField->calcCharOffset(), "TotalCharOffset", xmlNode );
        serializeNodeProp( _textField->calcCharScale(), "TotalCharScale", xmlNode );
        serializeNodeProp( (uint32_t)_textField->calcHorizontAlign(), "TotalHorizontAlign", xmlNode );
        serializeNodeProp( (uint32_t)_textField->calcVerticalAlign(), "TotalVerticalAlign", xmlNode );

        serializeNodeProp( _textField->getMaxCharCount(), "MaxCharCount", xmlNode );
        serializeNodeProp( _textField->getPixelsnap(), "Pixelsnap", xmlNode );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::serializeNode( const NodePtr & _node, pugi::xml_node & _xmlParentNode )
    {
        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Node" );

        xmlNode.append_attribute( "uid" ).set_value( _node->getUniqueIdentity() );
        xmlNode.append_attribute( "name" ).set_value( _node->getName().c_str() );
        xmlNode.append_attribute( "type" ).set_value( _node->getType().c_str() );

        serializeNodeProp( _node->isEnable(), "enable", xmlNode );

        this->serializeTransformation( _node, xmlNode );

        RenderInterface * render = _node->getRender();

        if( render != nullptr )
        {
            this->serializeRender( render, xmlNode );
        }

        AnimationInterface * animation = _node->getAnimation();
        if( animation != nullptr )
        {
            this->serializeAnimation( animation, xmlNode );
        }

        TextFieldPtr textField = stdex::intrusive_dynamic_cast<TextFieldPtr>(_node);
        if( textField != nullptr )
        {
            this->serializeTextField( textField, xmlNode );
        }

        if( _node->emptyChildren() == false )
        {
            pugi::xml_node xmlChildrenContainer = xmlNode.append_child( "Children" );

            _node->foreachChildren( [this, &xmlChildrenContainer]( const NodePtr & _child )
            {
                this->serializeNode( _child, xmlChildrenContainer );
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::sendScene( const ScenePtr & _scene )
    {
        pugi::xml_document doc;

        pugi::xml_node packetNode = doc.append_child( "Packet" );
        packetNode.append_attribute( "type" ).set_value( "Scene" );

        if( _scene != nullptr )
        {
            pugi::xml_node payloadNode = packetNode.append_child( "Payload" );

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
    void NodeDebuggerModule::processPacket( NodeDebuggerPacket & _packet )
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
        else if( typeStr == "GameControl" )
        {
            pugi::xml_node xmlNode = payloadNode.child( "Command" );
            if( xmlNode )
            {
                pugi::xml_attribute valueAttr = xmlNode.attribute( "value" );

                if( valueAttr )
                {
                    receiveGameControlCommand( valueAttr.as_string() );
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::receiveChangedNode( const pugi::xml_node & _xmlNode )
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

            pugi::xml_node transformationNode = _xmlNode.child( "Transformation" );

            deserializeNodeProp<mt::vec3f>( "position", transformationNode, [node]( const mt::vec3f & _value )
            {
                node->setLocalPosition( _value );
            } );

            deserializeNodeProp<mt::vec3f>( "origin", transformationNode, [node]( const mt::vec3f & _value )
            {
                node->setOrigin( _value );
            } );

            deserializeNodeProp<mt::vec2f>( "skew", transformationNode, [node]( const mt::vec2f & _value )
            {
                node->setSkew( _value );
            } );

            deserializeNodeProp<mt::vec3f>( "scale", transformationNode, [node]( const mt::vec3f & _value )
            {
                node->setScale( _value );
            } );

            deserializeNodeProp<mt::vec3f>( "orientation", transformationNode, [node]( const mt::vec3f & _value )
            {
                node->setOrientation( _value );
            } );

            pugi::xml_node renderNode = _xmlNode.child( "Render" );

            if( renderNode )
            {
                RenderInterface * render = node->getRender();

                deserializeNodeProp<bool>( "hide", renderNode, [render]( bool _value )
                {
                    render->setHide( _value );
                } );

                deserializeNodeProp<Color>( "color", renderNode, [render]( Color _value )
                {
                    render->setLocalColor( _value );
                } );
            }

            pugi::xml_node animationNode = _xmlNode.child( "Animation" );
            
            if( animationNode )
            {
                AnimationInterface * animation = node->getAnimation();

                deserializeNodeProp<bool>( "loop", animationNode, [animation]( bool _value )
                {
                    animation->setLoop( _value );
                } );
            }

            pugi::xml_node typeTextFieldNode = _xmlNode.child( "Type:TextField" );

            if( typeTextFieldNode )
            {
                TextFieldPtr textField = stdex::intrusive_static_cast<TextFieldPtr>(node);

                deserializeNodeProp<float>( "MaxLength", typeTextFieldNode, [textField]( auto _value )
                {
                    textField->setMaxLength( _value );
                } );

                deserializeNodeProp<bool>( "Wrap", typeTextFieldNode, [textField]( auto _value )
                {
                    textField->setWrap( _value );
                } );

                deserializeNodeProp<ConstString>( "TextID", typeTextFieldNode, [textField]( auto _value )
                {
                    textField->setTextID( _value );
                } );

                deserializeNodeProp<ConstString>( "TextAliasEnvironment", typeTextFieldNode, [textField]( auto _value )
                {
                    textField->setTextAliasEnvironment( _value );
                } );

                deserializeNodeProp<ConstString>( "FontName", typeTextFieldNode, [textField]( auto _value )
                {
                    textField->setFontName( _value );
                } );

                deserializeNodeProp<Color>( "FontColor", typeTextFieldNode, [textField]( auto _value )
                {
                    textField->setFontColor( _value );
                } );

                deserializeNodeProp<float>( "LineOffset", typeTextFieldNode, [textField]( auto _value )
                {
                    textField->setLineOffset( _value );
                } );

                deserializeNodeProp<float>( "CharOffset", typeTextFieldNode, [textField]( auto _value )
                {
                    textField->setCharOffset( _value );
                } );

                deserializeNodeProp<float>( "CharScale", typeTextFieldNode, [textField]( auto _value )
                {
                    textField->setCharScale( _value );
                } );

                deserializeNodeProp<uint32_t>( "HorizontAlign", typeTextFieldNode, [textField]( auto _value )
                {
                    textField->setHorizontAlign( (ETextHorizontAlign)_value );
                } );

                deserializeNodeProp<uint32_t>( "VerticalAlign", typeTextFieldNode, [textField]( auto _value )
                {
                    textField->setVerticalAlign( (ETextVerticalAlign)_value );
                } );

                deserializeNodeProp<uint32_t>( "MaxCharCount", typeTextFieldNode, [textField]( auto _value )
                {
                    textField->setMaxCharCount( _value );
                } );

                deserializeNodeProp<bool>( "Pixelsnap", typeTextFieldNode, [textField]( auto _value )
                {
                    textField->setPixelsnap( _value );
                } );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::receiveGameControlCommand( const String & _command )
    {
        if( _command == "pause" )
        {
            const bool alreadyFrozen = APPLICATION_SERVICE()
                ->isFrozen();

            APPLICATION_SERVICE()
                ->setFreeze( !alreadyFrozen );
        }
        else if( _command == "scene" )
        {
            m_shouldUpdateScene = true;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    VectorNodePath NodeDebuggerModule::stringToPath( const String & _str ) const
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
    void NodeDebuggerModule::notifyChangeScene( const ScenePtr & _scene )
    {
        this->setScene( _scene );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::notifyRemoveSceneDestroy()
    {
        this->setScene( nullptr );
    }
}