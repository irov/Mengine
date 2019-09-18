#include "NodeDebuggerModule.h"

#include "Interface/OptionsServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"
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
#include "Interface/ArchiveServiceInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/PickerInterface.h"
#include "Interface/AnimationInterface.h"

#include "NodeDebuggerSerialization.h"

#include "HotSpotPolygonDebuggerBoundingBox.h"
#include "TextFieldDebuggerBoundingBox.h"

#include "Kernel/Assertion.h"
#include "Kernel/ThreadTask.h"
#include "Kernel/SchedulerHelper.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/AssertionVocabulary.h"
#include "Kernel/NodeRenderHierarchy.h"
#include "Kernel/Scene.h"
#include "Kernel/StringFormat.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/GlobalInputHandlerHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

#include "Config/Stringstream.h"

#include "stdex/allocator_report.h"

#include <iomanip>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    NodeDebuggerModule::NodeDebuggerModule()
        : m_serverState( ENodeDebuggerServerState::Invalid )
        , m_shouldRecreateServer( false )
        , m_shouldUpdateScene( false )
        , m_workerId( 0 )
        , m_globalKeyHandlerF2( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    NodeDebuggerModule::~NodeDebuggerModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeDebuggerModule::_initializeModule()
    {
        VOCABULARY_SET( NodeDebuggerBoundingBoxInterface, STRINGIZE_STRING_LOCAL( "NodeDebuggerBoundingBox" ), STRINGIZE_STRING_LOCAL( "HotSpotPolygon" ), Helper::makeFactorableUnique<HotSpotPolygonDebuggerBoundingBox>() );
        VOCABULARY_SET( NodeDebuggerBoundingBoxInterface, STRINGIZE_STRING_LOCAL( "NodeDebuggerBoundingBox" ), STRINGIZE_STRING_LOCAL( "TextField" ), Helper::makeFactorableUnique<TextFieldDebuggerBoundingBox>() );

        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_CHANGE_SCENE_COMPLETE, this, &NodeDebuggerModule::notifyChangeScene );
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_REMOVE_SCENE_DESTROY, this, &NodeDebuggerModule::notifyRemoveSceneDestroy );

        m_globalKeyHandlerF2 = Helper::addGlobalKeyHandler( KC_F2, true, []( const InputKeyEvent & )
        {
            uint32_t exitCode;
            PLATFORM_SERVICE()
                ->createProcess( "NodeDebugger.exe", "127.0.0.1:18790", false, &exitCode );
        }, MENGINE_DOCUMENT_FUNCTION );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::_finalizeModule()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "NodeDebuggerBoundingBox" ), STRINGIZE_STRING_LOCAL( "HotSpotPolygon" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "NodeDebuggerBoundingBox" ), STRINGIZE_STRING_LOCAL( "TextField" ) );

        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "NodeDebuggerBoundingBox" ) );

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_SCENE_COMPLETE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_REMOVE_SCENE_DESTROY );

        if( m_threadJob != nullptr )
        {
            m_threadJob->removeWorker( m_workerId );

            THREAD_SERVICE()
                ->joinTask( m_threadJob );

            m_threadJob = nullptr;
        }

        const GlobalInputHandlerInterfacePtr & globalHandleSystem = PLAYER_SERVICE()
            ->getGlobalInputHandler();

        globalHandleSystem->removeGlobalHandler( m_globalKeyHandlerF2 );
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeDebuggerModule::_availableModule() const
    {
        if( SERVICE_EXIST( SocketSystemInterface ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeDebuggerModule::onWork( uint32_t )
    {
        switch( m_serverState )
        {
        case ENodeDebuggerServerState::Invalid:
            {
            }break;
        case ENodeDebuggerServerState::WaitingForClient:
            {
                int32_t check = m_socket->checkForClientConnection();
                if( check < 0 )
                {
                    // failed
                    m_serverState = ENodeDebuggerServerState::Invalid;
                    return false;
                }
                else if( check > 0 )
                {
                    // got client connection
                    m_serverState = ENodeDebuggerServerState::Connected;

                    APPLICATION_SERVICE()
                        ->setNopause( true );

                    m_shouldUpdateScene = true;
                }
            } break;
        case ENodeDebuggerServerState::Connected:
            {
                // check if need to send data
                m_dataMutex->lock();
                {
                    if( !m_outgoingPackets.empty() )
                    {
                        for( const auto & p : m_outgoingPackets )
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
                        m_serverState = ENodeDebuggerServerState::Invalid;
                        return true;
                    }
                }

                // check if we have read something
                if( m_receivedData.size() > sizeof( PacketHeader ) )
                {
                    ThreadMutexScope mutexLock( m_dataMutex );

                    // check if we have enough data to form a packet
                    PacketHeader * hdr = reinterpret_cast<PacketHeader *>(m_receivedData.data());
                    while( hdr != nullptr && hdr->compressedSize <= (m_receivedData.size() - sizeof( PacketHeader )) )
                    {
                        // received garbage - nothing fancy, just disconnect
                        if( hdr->magic != PACKET_MAGIC )
                        {
                            m_shouldRecreateServer = true;
                            m_serverState = ENodeDebuggerServerState::Invalid;
                            return true;
                        }

                        const size_t dataSizeWithHeader = hdr->compressedSize + sizeof( PacketHeader );

                        NodeDebuggerPacket packet;
                        uncompressPacket( packet, *hdr, m_receivedData.data() + sizeof( PacketHeader ) );

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

        if( m_serverState == ENodeDebuggerServerState::Connected )
        {
            this->sendScene( m_scene );
            this->sendPickerable( m_scene );
            this->sendRenderable( m_scene );
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
                NodeDebuggerPacket & packet = m_incomingPackets.front();
                this->processPacket( packet );
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

        NodeDebuggerBoundingBoxInterfacePtr boundingBox = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "NodeDebuggerBoundingBox" ), type );

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

        VectorRenderVertex2D & vertices = RENDER_SERVICE()
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

        VectorRenderIndex & indices = RENDER_SERVICE()
            ->getDebugRenderIndex( 24 );

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
                , vertices.data(), vertices.size()
                , indices.data(), indices.size()
                , &bbox
                , false, MENGINE_DOCUMENT_FUNCTION );
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeDebuggerModule::privateInit()
    {
        m_shouldRecreateServer = true;

        m_threadJob = THREAD_SERVICE()
            ->createJob( 50u, MENGINE_DOCUMENT_FUNCTION );

        THREAD_SERVICE()
            ->createThread( STRINGIZE_STRING_LOCAL( "NodeDebuggerListenThread" ), MENGINE_THREAD_PRIORITY_BELOW_NORMAL, MENGINE_DOCUMENT_FUNCTION );

        THREAD_SERVICE()
            ->addTask( STRINGIZE_STRING_LOCAL( "NodeDebuggerListenThread" ), m_threadJob );

        m_dataMutex = THREAD_SERVICE()
            ->createMutex( MENGINE_DOCUMENT_FUNCTION );

        m_workerId = m_threadJob->addWorker( ThreadWorkerInterfacePtr( this ) );

        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( archivator, false );

        m_archivator = archivator;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::recreateServer()
    {
        m_socket = SOCKET_SYSTEM()
            ->createSocket( MENGINE_DOCUMENT_FUNCTION );

        SocketConnectInfo sci = { "0.0.0.0", "18790" };
        m_socket->bind( sci, false );

        m_serverState = ENodeDebuggerServerState::WaitingForClient;

        m_receivedData.resize( 0 );
        m_incomingPackets.resize( 0 );
        m_outgoingPackets.resize( 0 );

        m_shouldRecreateServer = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::compressPacket( NodeDebuggerPacket & _packet, PacketHeader & _hdr )
    {
        const size_t payloadSize = _packet.payload.size();

        if( m_archivator == nullptr || payloadSize < 1024 )
        {
            _hdr.compressedSize = static_cast<uint32_t>(payloadSize);
            _hdr.uncompressedSize = 0; // packet is not compressed
        }
        else
        {
            const size_t maxCompressedSize = m_archivator->compressBound( payloadSize );
            Vector<uint8_t> compressedPayload( maxCompressedSize );
            size_t compressedSize = 0;
            const bool success = m_archivator->compress( compressedPayload.data(), maxCompressedSize, _packet.payload.data(), payloadSize, compressedSize, EAC_NORMAL );
            if( success == false || compressedSize >= payloadSize )
            {
                _hdr.compressedSize = static_cast<uint32_t>(payloadSize);
                _hdr.uncompressedSize = 0; // packet is not compressed
            }
            else
            {
                _hdr.compressedSize = static_cast<uint32_t>(compressedSize);
                _hdr.uncompressedSize = static_cast<uint32_t>(payloadSize);

                compressedPayload.resize( compressedSize );
                _packet.payload.swap( compressedPayload );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::uncompressPacket( NodeDebuggerPacket & _packet, PacketHeader & _hdr, const uint8_t * _receivedData )
    {
        if( _hdr.uncompressedSize == 0 )
        {
            // this packet is uncompressed, just copy
            _packet.payload.resize( _hdr.compressedSize );
            memcpy( _packet.payload.data(), _receivedData, _hdr.compressedSize );
        }
        else
        {
            _packet.payload.resize( _hdr.uncompressedSize );
            size_t uncompressedDataSize = 0;
            const bool success = m_archivator->decompress( _packet.payload.data(), _hdr.uncompressedSize, _receivedData, _hdr.compressedSize, uncompressedDataSize );
            MENGINE_ASSERTION( success == true && uncompressedDataSize == _hdr.uncompressedSize, "Packet decompression failed!" );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::sendPacket( NodeDebuggerPacket & _packet )
    {
        if( !_packet.payload.empty() )
        {
            PacketHeader hdr;
            hdr.magic = PACKET_MAGIC;

            compressPacket( _packet, hdr );

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
        serializeNodeProp( _transformation->getLocalOrigin(), "origin", xmlNode );
        serializeNodeProp( _transformation->getLocalSkew(), "skew", xmlNode );
        serializeNodeProp( _transformation->getLocalScale(), "scale", xmlNode );
        serializeNodeProp( _transformation->getLocalOrientation(), "orientation", xmlNode );
        serializeNodeProp( _transformation->getWorldPosition(), "worldPosition", xmlNode );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::serializeRender( const RenderInterface * _render, pugi::xml_node & _xmlParentNode )
    {
        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Render" );

        serializeNodeProp( _render->isRenderEnable(), "enable", xmlNode );
        serializeNodeProp( _render->isHide(), "hide", xmlNode );
        serializeNodeProp( _render->getLocalColor(), "local_color", xmlNode );
        serializeNodeProp( _render->getPersonalColor(), "personal_color", xmlNode );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::serializeAnimation( const AnimationInterface * _animation, pugi::xml_node & _xmlParentNode )
    {
        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Animation" );

        serializeNodeProp( _animation->isLoop(), "loop", xmlNode );
        serializeNodeProp( _animation->isPlay(), "play", xmlNode );
        serializeNodeProp( _animation->isPause(), "pause", xmlNode );
        serializeNodeProp( _animation->getTime(), "time", xmlNode );
        serializeNodeProp( _animation->getDuration(), "duration", xmlNode );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::serializeTextField( const TextFieldPtr & _textField, pugi::xml_node & _xmlParentNode )
    {
        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Type:TextField" );

        serializeNodeProp( _textField->getMaxLength(), "MaxLength", xmlNode );
        serializeNodeProp( _textField->getWrap(), "Wrap", xmlNode );

        const ConstString & textID = _textField->getTextID();
        const ConstString & textAliasEnvironment = _textField->getTextAliasEnvironment();

        const ConstString & aliasTestId = TEXT_SERVICE()
            ->getTextAlias( textAliasEnvironment, textID );

        serializeNodeProp( aliasTestId, "TextID", xmlNode );
        serializeNodeProp( textAliasEnvironment, "TextAliasEnvironment", xmlNode );

        TextEntryInterfacePtr textEntry;
        if( TEXT_SERVICE()
            ->hasTextEntry( aliasTestId, &textEntry ) == false )
        {
            serializeNodeProp( false, "HasText", xmlNode );
        }
        else
        {
            serializeNodeProp( true, "HasText", xmlNode );

            size_t textSize;
            const Char * textValue = textEntry->getValue( &textSize );

            serializeNodeProp( String( textValue, textSize ), "Format", xmlNode );

            VectorString textFormatArgs = _textField->getTextFormatArgs();

            TEXT_SERVICE()
                ->getTextAliasArguments( textAliasEnvironment, textID, textFormatArgs );

            String fmt;
            Helper::getStringFormat( fmt, textValue, textSize, textFormatArgs );

            serializeNodeProp( fmt, "Text", xmlNode );

            serializeNodeProp( _textField->calcFontName(), "TotalFontName", xmlNode );
            serializeNodeProp( _textField->calcFontColor(), "TotalFontColor", xmlNode );
            serializeNodeProp( _textField->calcLineOffset(), "TotalLineOffset", xmlNode );
            serializeNodeProp( _textField->calcCharOffset(), "TotalCharOffset", xmlNode );
            serializeNodeProp( _textField->calcCharScale(), "TotalCharScale", xmlNode );
            serializeNodeProp( (uint32_t)_textField->calcHorizontAlign(), "TotalHorizontAlign", xmlNode );
            serializeNodeProp( (uint32_t)_textField->calcVerticalAlign(), "TotalVerticalAlign", xmlNode );
        }

        serializeNodeProp( _textField->getFontName(), "FontName", xmlNode );
        serializeNodeProp( _textField->getFontColor(), "FontColor", xmlNode );
        serializeNodeProp( _textField->getLineOffset(), "LineOffset", xmlNode );
        serializeNodeProp( _textField->getCharOffset(), "CharOffset", xmlNode );
        serializeNodeProp( _textField->getCharScale(), "CharScale", xmlNode );
        serializeNodeProp( (uint32_t)_textField->getHorizontAlign(), "HorizontAlign", xmlNode );
        serializeNodeProp( (uint32_t)_textField->getVerticalAlign(), "VerticalAlign", xmlNode );

        serializeNodeProp( _textField->getMaxCharCount(), "MaxCharCount", xmlNode );
        serializeNodeProp( _textField->getPixelsnap(), "Pixelsnap", xmlNode );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::serializeMovie2( UnknownMovie2Interface * _unknownMovie2, pugi::xml_node & _xmlParentNode )
    {
        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Type:Movie2" );

        serializeNodeProp( _unknownMovie2->getCompositionName(), "CompositionName", xmlNode );
        serializeNodeProp( _unknownMovie2->getTextAliasEnvironment(), "TextAliasEnvironment", xmlNode );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::serializeNode( const NodePtr & _node, pugi::xml_node & _xmlParentNode )
    {
        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Node" );

        this->serializeNodeSingle( _node, xmlNode );

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
    void NodeDebuggerModule::serializeNodeSingle( const NodePtr & _node, pugi::xml_node & _xmlNode )
    {
        serializeNodeProp( _node->getUniqueIdentity(), "uid", _xmlNode );
        serializeNodeProp( _node->getName(), "name", _xmlNode );
        serializeNodeProp( _node->getType(), "type", _xmlNode );
        serializeNodeProp( _node->isEnable(), "enable", _xmlNode );

        this->serializeTransformation( _node, _xmlNode );

        RenderInterface * render = _node->getRender();

        if( render != nullptr )
        {
            this->serializeRender( render, _xmlNode );
        }

        AnimationInterface * animation = _node->getAnimation();
        if( animation != nullptr )
        {
            this->serializeAnimation( animation, _xmlNode );
        }

        TextFieldPtr textField = stdex::intrusive_dynamic_cast<TextFieldPtr>(_node);
        if( textField != nullptr )
        {
            this->serializeTextField( textField, _xmlNode );
        }

        UnknownInterface * unknownNode = _node->getUnknown();

        UnknownMovie2Interface * unknownMovie2 = dynamic_cast<UnknownMovie2Interface *>(unknownNode);
        if( unknownMovie2 != nullptr )
        {
            this->serializeMovie2( unknownMovie2, _xmlNode );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::serializePickerable( PickerInterface * _picker, pugi::xml_node & _xmlParentNode )
    {
        Node * nodeChild = dynamic_cast<Node *>(_picker);

        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Node" );

        if( nodeChild != nullptr )
        {
            this->serializeNodeSingle( NodePtr::from( nodeChild ), xmlNode );
        }

        if( _picker->emptyPickerChildren() == false )
        {
            pugi::xml_node xmlChildrenContainer = xmlNode.append_child( "Children" );

            _picker->foreachPickerChildren( [this, &xmlChildrenContainer]( PickerInterface * _child )
            {
                this->serializePickerable( _child, xmlChildrenContainer );
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::serializeRenderable( RenderInterface * _render, pugi::xml_node & _xmlParentNode )
    {
        Node * nodeChild = dynamic_cast<Node *>(_render);

        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Node" );

        if( nodeChild != nullptr )
        {
            this->serializeNodeSingle( NodePtr::from( nodeChild ), xmlNode );
        }

        if( _render->emptyRenderChildren() == false )
        {
            pugi::xml_node xmlChildrenContainer = xmlNode.append_child( "Children" );

            _render->foreachRenderChildren( [this, &xmlChildrenContainer]( RenderInterface * _child )
            {
                this->serializeRenderable( _child, xmlChildrenContainer );
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

            this->serializeNode( _scene, payloadNode );
        }

        NodeDebuggerPacket packet;

        MyXMLWriter writer( packet.payload );

#ifdef _DEBUG
        const uint32_t xmlFlags = pugi::format_indent;
#else
        const uint32_t xmlFlags = pugi::format_raw;
#endif
        doc.save( writer, "  ", xmlFlags, pugi::encoding_utf8 );

        this->sendPacket( packet );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::sendPickerable( const ScenePtr & _scene )
    {
        pugi::xml_document doc;

        pugi::xml_node packetNode = doc.append_child( "Packet" );
        packetNode.append_attribute( "type" ).set_value( "Pickerable" );

        if( _scene != nullptr )
        {
            pugi::xml_node payloadNode = packetNode.append_child( "Payload" );

            PickerInterface * picker = _scene->getPicker();

            this->serializePickerable( picker, payloadNode );
        }

        NodeDebuggerPacket packet;

        MyXMLWriter writer( packet.payload );

#ifdef _DEBUG
        const uint32_t xmlFlags = pugi::format_indent;
#else
        const uint32_t xmlFlags = pugi::format_raw;
#endif
        doc.save( writer, "  ", xmlFlags, pugi::encoding_utf8 );

        this->sendPacket( packet );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::sendRenderable( const ScenePtr & _scene )
    {
        pugi::xml_document doc;

        pugi::xml_node packetNode = doc.append_child( "Packet" );
        packetNode.append_attribute( "type" ).set_value( "Renderable" );

        if( _scene != nullptr )
        {
            pugi::xml_node payloadNode = packetNode.append_child( "Payload" );

            RenderInterface * render = _scene->getRender();

            this->serializeRenderable( render, payloadNode );
        }

        NodeDebuggerPacket packet;

        MyXMLWriter writer( packet.payload );

#ifdef _DEBUG
        const uint32_t xmlFlags = pugi::format_indent;
#else
        const uint32_t xmlFlags = pugi::format_raw;
#endif
        doc.save( writer, "  ", xmlFlags, pugi::encoding_utf8 );

        this->sendPacket( packet );
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
                node->setLocalOrigin( _value );
            } );

            deserializeNodeProp<mt::vec2f>( "skew", transformationNode, [node]( const mt::vec2f & _value )
            {
                node->setLocalSkew( _value );
            } );

            deserializeNodeProp<mt::vec3f>( "scale", transformationNode, [node]( const mt::vec3f & _value )
            {
                node->setLocalScale( _value );
            } );

            deserializeNodeProp<mt::vec3f>( "orientation", transformationNode, [node]( const mt::vec3f & _value )
            {
                node->setLocalOrientation( _value );
            } );

            pugi::xml_node renderNode = _xmlNode.child( "Render" );

            if( renderNode )
            {
                RenderInterface * render = node->getRender();

                deserializeNodeProp<bool>( "hide", renderNode, [render]( bool _value )
                {
                    render->setHide( _value );
                } );

                deserializeNodeProp<Color>( "local_color", renderNode, [render]( const Color & _value )
                {
                    render->setLocalColor( _value );
                } );

                deserializeNodeProp<Color>( "personal_color", renderNode, [render]( const Color & _value )
                {
                    render->setPersonalColor( _value );
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

                deserializeNodeProp<bool>( "time", animationNode, [animation]( float _value )
                {
                    if( _value < 0.f )
                    {
                        _value = 0.f;
                    }
                    else if( _value > animation->getDuration() )
                    {
                        _value = animation->getDuration();
                    }

                    animation->setTime( _value );
                } );
            }

            pugi::xml_node typeNodeTextField = _xmlNode.child( "Type:TextField" );

            if( typeNodeTextField )
            {
                TextFieldPtr textField = stdex::intrusive_static_cast<TextFieldPtr>(node);

                deserializeNodeProp<float>( "MaxLength", typeNodeTextField, [textField]( auto _value )
                {
                    textField->setMaxLength( _value );
                } );

                deserializeNodeProp<bool>( "Wrap", typeNodeTextField, [textField]( auto _value )
                {
                    textField->setWrap( _value );
                } );

                deserializeNodeProp<ConstString>( "TextID", typeNodeTextField, [textField]( auto _value )
                {
                    textField->setTextID( _value );
                } );

                deserializeNodeProp<ConstString>( "TextAliasEnvironment", typeNodeTextField, [textField]( auto _value )
                {
                    textField->setTextAliasEnvironment( _value );
                } );

                deserializeNodeProp<ConstString>( "FontName", typeNodeTextField, [textField]( auto _value )
                {
                    textField->setFontName( _value );
                } );

                deserializeNodeProp<Color>( "FontColor", typeNodeTextField, [textField]( auto _value )
                {
                    textField->setFontColor( _value );
                } );

                deserializeNodeProp<float>( "LineOffset", typeNodeTextField, [textField]( auto _value )
                {
                    textField->setLineOffset( _value );
                } );

                deserializeNodeProp<float>( "CharOffset", typeNodeTextField, [textField]( auto _value )
                {
                    textField->setCharOffset( _value );
                } );

                deserializeNodeProp<float>( "CharScale", typeNodeTextField, [textField]( auto _value )
                {
                    textField->setCharScale( _value );
                } );

                deserializeNodeProp<uint32_t>( "HorizontAlign", typeNodeTextField, [textField]( auto _value )
                {
                    textField->setHorizontAlign( (ETextHorizontAlign)_value );
                } );

                deserializeNodeProp<uint32_t>( "VerticalAlign", typeNodeTextField, [textField]( auto _value )
                {
                    textField->setVerticalAlign( (ETextVerticalAlign)_value );
                } );

                deserializeNodeProp<uint32_t>( "MaxCharCount", typeNodeTextField, [textField]( auto _value )
                {
                    textField->setMaxCharCount( _value );
                } );

                deserializeNodeProp<bool>( "Pixelsnap", typeNodeTextField, [textField]( auto _value )
                {
                    textField->setPixelsnap( _value );
                } );
            }

            pugi::xml_node typeNodeMovie2 = _xmlNode.child( "Type:Movie2" );

            if( typeNodeMovie2 )
            {
                UnknownMovie2Interface * unknownMovie2 = node->getUnknown();

                deserializeNodeProp<ConstString>( "TextAliasEnvironment", typeNodeMovie2, [unknownMovie2]( auto _value )
                {
                    unknownMovie2->setTextAliasEnvironment( _value );
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
            const Char * ptr = _str.c_str();

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
