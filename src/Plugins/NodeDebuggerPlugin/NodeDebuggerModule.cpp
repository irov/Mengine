#include "NodeDebuggerModule.h"

#include "Interface/OptionsServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/RenderServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/SoundSystemInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/FactoryServiceInterface.h"
#include "Interface/SocketSystemInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/FontServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/ArchiveServiceInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/PickerInterface.h"
#include "Interface/AnimationInterface.h"
#include "Interface/SettingsServiceInterface.h"
#include "Interface/AllocatorServiceInterface.h"

#ifdef MENGINE_ENVIRONMENT_PLATFORM_WIN32
#   include "Interface/Win32PlatformExtensionInterface.h"
#endif

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
#include "Kernel/Surface.h"
#include "Kernel/StringFormat.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/GlobalInputHandlerHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Stringstream.h"
#include "Kernel/Blobject.h"

#include "Config/StdString.h"

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
        , m_requestListenerId( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    NodeDebuggerModule::~NodeDebuggerModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeDebuggerModule::_initializeModule()
    {
        VOCABULARY_SET( DebuggerBoundingBoxInterface, STRINGIZE_STRING_LOCAL( "DebuggerBoundingBox" ), STRINGIZE_STRING_LOCAL( "HotSpotPolygon" ), Helper::makeFactorableUnique<HotSpotPolygonDebuggerBoundingBox>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( DebuggerBoundingBoxInterface, STRINGIZE_STRING_LOCAL( "DebuggerBoundingBox" ), STRINGIZE_STRING_LOCAL( "HotSpotSurface" ), Helper::makeFactorableUnique<HotSpotPolygonDebuggerBoundingBox>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( DebuggerBoundingBoxInterface, STRINGIZE_STRING_LOCAL( "DebuggerBoundingBox" ), STRINGIZE_STRING_LOCAL( "TextField" ), Helper::makeFactorableUnique<TextFieldDebuggerBoundingBox>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CHANGE_ARROW_COMPLETE, &NodeDebuggerModule::notifyChangeArrow, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CHANGE_SCENE_DESTROY, &NodeDebuggerModule::notifyChangeSceneDestroy, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CHANGE_SCENE_COMPLETE, &NodeDebuggerModule::notifyChangeSceneComplete, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_REMOVE_SCENE_DESTROY, &NodeDebuggerModule::notifyRemoveSceneDestroy, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_INCREF_FACTORY_GENERATION, &NodeDebuggerModule::notifyIncrefFactoryGeneration, MENGINE_DOCUMENT_FACTORABLE );

#ifdef MENGINE_ENVIRONMENT_PLATFORM_WIN32
        uint32_t globalKeyHandlerF2 = Helper::addGlobalKeyHandler( KC_F2, true, []( const InputKeyEvent & )
        {
            Win32PlatformExtensionInterface * win32Platform = PLATFORM_SERVICE()
                ->getPlatformExtention();

            uint32_t exitCode;
            win32Platform->createProcess( "NodeDebugger.exe", "127.0.0.1:18790", false, &exitCode );
        }, MENGINE_DOCUMENT_FACTORABLE );

        m_globalKeyHandlerF2 = globalKeyHandlerF2;
#endif

        cURLRequestListenerPtr logger = Helper::makeFactorableUnique<cURLRequestListener>( MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ASSERTION_FATAL( logger != nullptr );

        logger->setSceneDataProvider( SceneDataProviderInterfacePtr::from( this ));

        m_networkLogger = logger;

        if( SERVICE_EXIST( cURLServiceInterface ) == true )
        {
            m_requestListenerId = CURL_SERVICE()
                ->addRequestListener( m_networkLogger, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_FATAL( m_requestListenerId != INVALIDATE_UNIQUE_ID );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::_finalizeModule()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "DebuggerBoundingBox" ), STRINGIZE_STRING_LOCAL( "HotSpotPolygon" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "DebuggerBoundingBox" ), STRINGIZE_STRING_LOCAL( "HotSpotSurface" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "DebuggerBoundingBox" ), STRINGIZE_STRING_LOCAL( "TextField" ) );

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_ARROW_COMPLETE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_SCENE_DESTROY );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_SCENE_COMPLETE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_REMOVE_SCENE_DESTROY );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_INCREF_FACTORY_GENERATION );

        if( m_threadJob != nullptr )
        {
            m_threadJob->removeWorker( m_workerId );

            THREAD_SERVICE()
                ->joinTask( m_threadJob );

            m_threadJob = nullptr;
        }

        THREAD_SERVICE()
            ->destroyThread( STRINGIZE_STRING_LOCAL( "NodeDebuggerListenThread" ) );

        m_arrow = nullptr;
        m_scene = nullptr;

        if( m_socket != nullptr )
        {
            m_socket->disconnect();
            m_socket = nullptr;
        }

        m_dataMutex = nullptr;

        m_archivator = nullptr;

        if( m_globalKeyHandlerF2 != 0 )
        {
            Helper::removeGlobalHandler( m_globalKeyHandlerF2 );
            m_globalKeyHandlerF2 = 0;
        }

        if( SERVICE_EXIST( cURLServiceInterface ) == true )
        {
            CURL_SERVICE()
                ->removeRequestListener( m_requestListenerId );
        }
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
    void NodeDebuggerModule::onThreadWorkerUpdate( uint32_t _id )
    {
        MENGINE_UNUSED( _id );
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeDebuggerModule::onThreadWorkerWork( uint32_t )
    {
        switch( m_serverState )
        {
        case ENodeDebuggerServerState::Invalid:
            {
            }break;
        case ENodeDebuggerServerState::WaitingForClient:
            {
                if( m_socket == nullptr )
                {
                    m_serverState = ENodeDebuggerServerState::Invalid;
                    return false;
                }

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
                Deque<NodeDebuggerPacket> outgoingPacketsAux = std::move( m_outgoingPackets );
                m_dataMutex->unlock();

                if( outgoingPacketsAux.empty() == false )
                {
                    for( const NodeDebuggerPacket & p : outgoingPacketsAux )
                    {
                        m_socket->send( p.payload.data(), p.payload.size() );
                    }
                }

                // now check if we received something (100 ms)
                bool haveSomeData = m_socket->waitForData( 100u );
                if( haveSomeData == true )
                {
                    static constexpr size_t BUFFER_SIZE = 4096;
                    uint8_t buffer[BUFFER_SIZE];

                    int32_t bytesReceived = 0;
                    bool clientDisconnected = false;
                    do
                    {
                        bytesReceived = m_socket->receive( buffer, BUFFER_SIZE );

                        if( bytesReceived > 0 )
                        {
                            m_receivedData.insert( m_receivedData.end(), buffer + 0, buffer + bytesReceived );
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
                        this->uncompressPacket( packet, *hdr, m_receivedData.data() + sizeof( PacketHeader ) );

                        m_dataMutex->lock();
                        m_incomingPackets.emplace_back( packet );
                        m_dataMutex->unlock();

                        // now remove this packet data from the buffer
                        const size_t newSize = m_receivedData.size() - dataSizeWithHeader;
                        if( newSize )
                        {
                            MENGINE_MEMMOVE( m_receivedData.data(), m_receivedData.data() + dataSizeWithHeader, newSize );
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
    void NodeDebuggerModule::onThreadWorkerDone( uint32_t )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::setArrow( const ArrowPtr & _arrow )
    {
        if( m_arrow != _arrow )
        {
            m_arrow = _arrow;

            m_shouldUpdateScene = true;
        }
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
        if( m_serverState == ENodeDebuggerServerState::Connected )
        {
            if( m_currentTab == "scene" )
            {
                this->sendArrow( m_arrow );
                this->sendScene( m_scene );
                this->sendPickerable( m_scene );
                this->sendRenderable( m_scene );
            }

            if( m_currentTab == "settings" )
            {
                this->sendSettings();
            }

            if( m_currentTab == "memory" )
            {
                this->sendMemory();
            }

            if( m_currentTab == "leak" )
            {
                this->sendObjectsLeak();
            }

            if( m_currentTab == "network" )
            {
                this->sendNetwork();
            }

            m_shouldUpdateScene = false;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::_update( bool _focus )
    {
        MENGINE_UNUSED( _focus );

        if( m_shouldRecreateServer == true )
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
            m_dataMutex->lock();
            if( m_incomingPackets.empty() == false )
            {
                NodeDebuggerPacket packet = std::move( m_incomingPackets.front() );
                m_incomingPackets.pop_front();
                m_dataMutex->unlock();

                this->processPacket( packet );
            }
            else
            {
                m_dataMutex->unlock();
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    static bool s_absorbBoundingBox( const NodePtr & _node, mt::box2f * _bb )
    {
        if( _node->isEnable() == false )
        {
            return false;
        }

        bool successul = false;

        mt::box2f absorb_bb;
        mt::insideout_box( absorb_bb );

        const ConstString & type = _node->getType();

        DebuggerBoundingBoxInterfacePtr boundingBox = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "DebuggerBoundingBox" ), type );

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
            if( s_absorbBoundingBox( _child, &child_bb ) == true )
            {
                mt::merge_box( absorb_bb, child_bb );

                successul = true;
            }
        } );

        *_bb = absorb_bb;

        return successul;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::_render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context )
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
        Helper::fillNodeRenderContext( node, &node_context );

        mt::box2f bbox;
        if( s_absorbBoundingBox( node, &bbox ) == false )
        {
            return;
        }

        const RenderCameraInterface * camera = node_context.camera;

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

        ColorValue_ARGB color = Helper::makeRGBA( 1.f, 0.f, 1.f, 1.f );

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

        _renderPipeline->addRenderObject( &node_context
            , debugMaterial
            , nullptr
            , vertices.data(), (uint32_t)vertices.size()
            , indices.data(), (uint32_t)indices.size()
            , &bbox
            , false, MENGINE_DOCUMENT_FORWARD );
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeDebuggerModule::privateInit()
    {
        m_shouldRecreateServer = true;

        m_threadJob = THREAD_SERVICE()
            ->createJob( 50u, MENGINE_DOCUMENT_FACTORABLE );

        THREAD_SERVICE()
            ->createThread( STRINGIZE_STRING_LOCAL( "NodeDebuggerListenThread" ), MENGINE_THREAD_PRIORITY_BELOW_NORMAL, MENGINE_DOCUMENT_FACTORABLE );

        THREAD_SERVICE()
            ->addTask( STRINGIZE_STRING_LOCAL( "NodeDebuggerListenThread" ), m_threadJob );

        m_dataMutex = THREAD_SERVICE()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        m_workerId = m_threadJob->addWorker( ThreadWorkerInterfacePtr( this ), MENGINE_DOCUMENT_FACTORABLE );

        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( archivator );

        m_archivator = archivator;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::recreateServer()
    {
        m_socket = SOCKET_SYSTEM()
            ->createSocket( MENGINE_DOCUMENT_FACTORABLE );

        SocketConnectInfo sci = {"0.0.0.0", "18790"};
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
            Blobject compressedPayload( maxCompressedSize );
            size_t compressedSize = 0;
            bool success = m_archivator->compress( compressedPayload.data(), maxCompressedSize, _packet.payload.data(), payloadSize, &compressedSize, EAC_NORMAL );
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
            MENGINE_MEMCPY( _packet.payload.data(), _receivedData, _hdr.compressedSize );
        }
        else
        {
            _packet.payload.resize( _hdr.uncompressedSize );
            size_t uncompressedDataSize = 0;
            bool success = m_archivator->decompress( _packet.payload.data(), _hdr.uncompressedSize, _receivedData, _hdr.compressedSize, &uncompressedDataSize );
            MENGINE_UNUSED( success );
            MENGINE_ASSERTION( success == true && uncompressedDataSize == _hdr.uncompressedSize, "Packet decompression failed!" );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::sendPacket( NodeDebuggerPacket & _packet )
    {
        if( _packet.payload.empty() == true )
        {
            return;
        }

        PacketHeader hdr;
        hdr.magic = PACKET_MAGIC;

        this->compressPacket( _packet, hdr );

        Detail::InsertPacketHeader( _packet.payload, hdr );

        ThreadMutexScope mutexLock( m_dataMutex );
        m_outgoingPackets.emplace_back( _packet );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::serializeTransformation( const TransformablePtr & _transformable, pugi::xml_node & _xmlParentNode )
    {
        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Transformation" );

        const TransformationInterface * transformation = _transformable->getTransformation();

        Detail::serializeNodeProp( transformation->getLocalPosition(), "position", xmlNode );
        Detail::serializeNodeProp( transformation->getLocalOrigin(), "origin", xmlNode );
        Detail::serializeNodeProp( transformation->getLocalSkew(), "skew", xmlNode );
        Detail::serializeNodeProp( transformation->getLocalScale(), "scale", xmlNode );
        Detail::serializeNodeProp( transformation->getLocalOrientation(), "orientation", xmlNode );
        Detail::serializeNodeProp( transformation->getWorldPosition(), "worldPosition", xmlNode );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::serializeRender( const RenderInterface * _render, pugi::xml_node & _xmlParentNode )
    {
        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Render" );

        Detail::serializeNodeProp( _render->isRenderEnable(), "enable", xmlNode );
        Detail::serializeNodeProp( _render->isHide(), "hide", xmlNode );
        Detail::serializeNodeProp( _render->getLocalColor(), "local_color", xmlNode );
        Detail::serializeNodeProp( _render->getPersonalColor(), "personal_color", xmlNode );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::serializeAnimation( const Compilable * _compilable, const AnimationInterface * _animation, pugi::xml_node & _xmlParentNode )
    {
        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Animation" );

        Detail::serializeNodeProp( _animation->isLoop(), "loop", xmlNode );
        Detail::serializeNodeProp( _animation->isPlay(), "play", xmlNode );
        Detail::serializeNodeProp( _animation->isPause(), "pause", xmlNode );

        if( _compilable->isCompile() == true )
        {
            Detail::serializeNodeProp( _animation->getTime(), "time", xmlNode );
        }
        else
        {
            Detail::serializeNodeProp( 0.f, "time", xmlNode );
        }

        Detail::serializeNodeProp( _animation->getDuration(), "duration", xmlNode );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::serializeTextField( const TextFieldPtr & _textField, pugi::xml_node & _xmlParentNode )
    {
        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Type:TextField" );

        Detail::serializeNodeProp( _textField->getMaxLength(), "MaxLength", xmlNode );
        Detail::serializeNodeProp( _textField->getWrap(), "Wrap", xmlNode );

        const ConstString & textID = _textField->getTextId();
        const ConstString & textAliasEnvironment = _textField->getTextAliasEnvironment();

        const ConstString & aliasTestId = TEXT_SERVICE()
            ->getTextAlias( textAliasEnvironment, textID );

        Detail::serializeNodeProp( aliasTestId, "TextID", xmlNode );
        Detail::serializeNodeProp( textAliasEnvironment, "TextAliasEnvironment", xmlNode );

        TextEntryInterfacePtr textEntry;
        if( TEXT_SERVICE()
            ->hasTextEntry( aliasTestId, &textEntry ) == false )
        {
            Detail::serializeNodeProp( false, "HasText", xmlNode );
        }
        else
        {
            Detail::serializeNodeProp( true, "HasText", xmlNode );

            size_t textSize;
            const Char * textValue = textEntry->getValue( &textSize );

            Detail::serializeNodeProp( String( textValue, textSize ), "Format", xmlNode );

            VectorString textFormatArgs = _textField->getTextFormatArgs();

            TEXT_SERVICE()
                ->getTextAliasArguments( textAliasEnvironment, textID, &textFormatArgs );

            String fmt;
            Helper::getStringFormat( &fmt, textValue, textSize, textFormatArgs );

            Detail::serializeNodeProp( fmt, "Text", xmlNode );

            Detail::serializeNodeProp( _textField->calcFont()->getName(), "TotalFontName", xmlNode );
            Detail::serializeNodeProp( _textField->calcFontColor(), "TotalFontColor", xmlNode );
            Detail::serializeNodeProp( _textField->calcLineOffset(), "TotalLineOffset", xmlNode );
            Detail::serializeNodeProp( _textField->calcCharOffset(), "TotalCharOffset", xmlNode );
            Detail::serializeNodeProp( _textField->calcCharScale(), "TotalCharScale", xmlNode );
            Detail::serializeNodeProp( (uint32_t)_textField->calcHorizontAlign(), "TotalHorizontAlign", xmlNode );
            Detail::serializeNodeProp( (uint32_t)_textField->calcVerticalAlign(), "TotalVerticalAlign", xmlNode );
        }

        Detail::serializeNodeProp( _textField->getFont()->getName(), "FontName", xmlNode );
        Detail::serializeNodeProp( _textField->getFontColor(), "FontColor", xmlNode );
        Detail::serializeNodeProp( _textField->getLineOffset(), "LineOffset", xmlNode );
        Detail::serializeNodeProp( _textField->getCharOffset(), "CharOffset", xmlNode );
        Detail::serializeNodeProp( _textField->getCharScale(), "CharScale", xmlNode );
        Detail::serializeNodeProp( (uint32_t)_textField->getHorizontAlign(), "HorizontAlign", xmlNode );
        Detail::serializeNodeProp( (uint32_t)_textField->getVerticalAlign(), "VerticalAlign", xmlNode );
        Detail::serializeNodeProp( _textField->getMaxCharCount(), "MaxCharCount", xmlNode );
        Detail::serializeNodeProp( _textField->getPixelsnap(), "Pixelsnap", xmlNode );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::serializeMovie2( const Compilable * _compilable, const UnknownMovie2Interface * _unknownMovie2, pugi::xml_node & _xmlParentNode )
    {
        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Type:Movie2" );

        Detail::serializeNodeProp( _unknownMovie2->getCompositionName(), "CompositionName", xmlNode );
        Detail::serializeNodeProp( _unknownMovie2->getTextAliasEnvironment(), "TextAliasEnvironment", xmlNode );

        pugi::xml_node xmlSubCompositions = xmlNode.append_child( "SubCompositions" );

        _unknownMovie2->foreachSubComposition( [this, _compilable, &xmlSubCompositions]( const Movie2SubCompositionInterfacePtr & _subComposition )
        {
            pugi::xml_node xmlElement = xmlSubCompositions.append_child( "Element" );

            Detail::serializeNodeProp( _subComposition->getName(), "SubCompositionName", xmlElement );
            Detail::serializeNodeProp( _subComposition->getSubCompositionEnable(), "SubCompositionEnable", xmlElement );

            AnimationInterface * animation = _subComposition->getAnimation();

            if( animation != nullptr )
            {
                this->serializeAnimation( _compilable, animation, xmlElement );
            }
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::serializeSpine( const UnknownSpineInterface * _unknownSpine, pugi::xml_node & _xmlParentNode )
    {
        const ResourcePtr & resourceSpineSkeleton = _unknownSpine->getResourceSpineSkeleton();

        if( resourceSpineSkeleton == nullptr )
        {
            return;
        }

        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Type:Spine" );

        Detail::serializeNodeProp( resourceSpineSkeleton->getName(), "ResourceName", xmlNode );
        Detail::serializeNodeProp( resourceSpineSkeleton->getType(), "ResourceType", xmlNode );

        const ContentInterface * content = resourceSpineSkeleton->getContent();

        if( content != nullptr )
        {
            this->serializeContent( content, xmlNode );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::serializeShape( const ShapePtr & _shape, pugi::xml_node & _xmlParentNode )
    {
        const SurfacePtr & surface = _shape->getSurface();

        if( surface == nullptr )
        {
            return;
        }

        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Component:Surface" );

        Detail::serializeNodeProp( surface->getName(), "Name", xmlNode );
        Detail::serializeNodeProp( surface->getType(), "Type", xmlNode );

        if( surface->isCompile() == true )
        {
            Detail::serializeNodeProp( true, "Compile", xmlNode );
            Detail::serializeNodeProp( surface->getMaxSize(), "MaxSize", xmlNode );
            Detail::serializeNodeProp( surface->getSize(), "Size", xmlNode );
            Detail::serializeNodeProp( surface->getOffset(), "Offset", xmlNode );
            Detail::serializeNodeProp( surface->getAnchor(), "Anchor", xmlNode );
        }
        else
        {
            Detail::serializeNodeProp( false, "Compile", xmlNode );
            Detail::serializeNodeProp( mt::vec2f{0.f, 0.f}, "MaxSize", xmlNode );
            Detail::serializeNodeProp( mt::vec2f{0.f, 0.f}, "Size", xmlNode );
            Detail::serializeNodeProp( mt::vec2f{0.f, 0.f}, "Offset", xmlNode );
            Detail::serializeNodeProp( mt::vec2f{0.f, 0.f}, "Anchor", xmlNode );
        }

        const AnimationInterface * animation = surface->getAnimation();

        if( animation != nullptr )
        {
            this->serializeAnimation( surface.get(), animation, xmlNode );
        }

        SurfaceImagePtr surfaceImage = stdex::intrusive_dynamic_cast<SurfaceImagePtr>(surface);

        if( surfaceImage != nullptr )
        {
            this->serializeSurfaceImage( surfaceImage, xmlNode );
        }

        SurfaceImageSequencePtr surfaceImageSequence = stdex::intrusive_dynamic_cast<SurfaceImageSequencePtr>(surface);

        if( surfaceImageSequence != nullptr )
        {
            this->serializeSurfaceImageSequence( surfaceImageSequence, xmlNode );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::serializeSurfaceImage( const SurfaceImagePtr & _surfaceImage, pugi::xml_node & _xmlParentNode )
    {
        const ResourceImagePtr & resourceImage = _surfaceImage->getResourceImage();

        if( resourceImage == nullptr )
        {
            return;
        }

        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Type:SurfaceImage" );

        Detail::serializeNodeProp( resourceImage->getName(), "ResourceName", xmlNode );
        Detail::serializeNodeProp( resourceImage->getType(), "ResourceType", xmlNode );

        const ContentInterface * content = resourceImage->getContent();

        if( content != nullptr )
        {
            this->serializeContent( content, xmlNode );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::serializeSurfaceImageSequence( const SurfaceImageSequencePtr & _surfaceImageSequence, pugi::xml_node & _xmlParentNode )
    {
        const ResourceImageSequencePtr & resourceImageSequence = _surfaceImageSequence->getResourceImageSequence();

        if( resourceImageSequence == nullptr )
        {
            return;
        }

        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Type:SurfaceImageSequence" );

        Detail::serializeNodeProp( resourceImageSequence->getName(), "ResourceName", xmlNode );
        Detail::serializeNodeProp( resourceImageSequence->getType(), "ResourceType", xmlNode );

        Detail::serializeNodeProp( resourceImageSequence->getSequenceCount(), "ResourceSequenceCount", xmlNode );
        Detail::serializeNodeProp( resourceImageSequence->getSequenceDuration(), "ResourceSequenceDuration", xmlNode );

        Detail::serializeNodeProp( resourceImageSequence->getSequenceCount(), "ResourceSequenceCount", xmlNode );
        Detail::serializeNodeProp( resourceImageSequence->getSequenceDuration(), "ResourceSequenceDuration", xmlNode );

        Detail::serializeNodeProp( _surfaceImageSequence->getCurrentFrame(), "CurrentFrame", xmlNode );

        const ContentInterface * content = resourceImageSequence->getContent();

        if( content != nullptr )
        {
            this->serializeContent( content, xmlNode );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::serializeContent( const ContentInterface * _content, pugi::xml_node & _xmlParentNode )
    {
        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Content" );

        const FileGroupInterfacePtr & fileGroup = _content->getFileGroup();

        Detail::serializeNodeProp( fileGroup->getName(), "FileGroup", xmlNode );

        Detail::serializeNodeProp( _content->getFilePath(), "FilePath", xmlNode );
        Detail::serializeNodeProp( _content->getCodecType(), "CodecType", xmlNode );
        Detail::serializeNodeProp( _content->getConverterType(), "ConverterType", xmlNode );
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
        Detail::serializeNodeProp( _node->getUniqueIdentity(), "uid", _xmlNode );
        Detail::serializeNodeProp( _node->getName(), "name", _xmlNode );
        Detail::serializeNodeProp( _node->getType(), "type", _xmlNode );
        Detail::serializeNodeProp( _node->isEnable(), "enable", _xmlNode );

        this->serializeTransformation( _node, _xmlNode );

        RenderInterface * render = _node->getRender();

        if( render != nullptr )
        {
            this->serializeRender( render, _xmlNode );
        }

        AnimationInterface * animation = _node->getAnimation();

        if( animation != nullptr )
        {
            this->serializeAnimation( _node.get(), animation, _xmlNode );
        }

        ShapePtr shape = stdex::intrusive_dynamic_cast<ShapePtr>(_node);

        if( shape != nullptr )
        {
            this->serializeShape( shape, _xmlNode );
        }

        TextFieldPtr textField = stdex::intrusive_dynamic_cast<TextFieldPtr>(_node);

        if( textField != nullptr )
        {
            this->serializeTextField( textField, _xmlNode );
        }

        const UnknownMovie2Interface * unknownMovie2 = _node->getDynamicUnknown();

        if( unknownMovie2 != nullptr )
        {
            this->serializeMovie2( _node.get(), unknownMovie2, _xmlNode );
        }

        const UnknownSpineInterface * unknownSpine = _node->getDynamicUnknown();

        if( unknownSpine != nullptr )
        {
            this->serializeSpine( unknownSpine, _xmlNode );
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
    void NodeDebuggerModule::sendArrow( const ArrowPtr & _arrow )
    {
        pugi::xml_document doc;

        pugi::xml_node packetNode = doc.append_child( "Packet" );
        packetNode.append_attribute( "type" ).set_value( "Arrow" );

        if( _arrow != nullptr )
        {
            pugi::xml_node payloadNode = packetNode.append_child( "Payload" );

            this->serializeNode( _arrow, payloadNode );
        }

        NodeDebuggerPacket packet;

        MyXMLWriter writer( packet.payload );

#ifdef MENGINE_DEBUG
        const uint32_t xmlFlags = pugi::format_indent;
#else
        const uint32_t xmlFlags = pugi::format_raw;
#endif
        doc.save( writer, "  ", xmlFlags, pugi::encoding_utf8 );

        this->sendPacket( packet );
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

#ifdef MENGINE_DEBUG
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

#ifdef MENGINE_DEBUG
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

#ifdef MENGINE_DEBUG
        const uint32_t xmlFlags = pugi::format_indent;
#else
        const uint32_t xmlFlags = pugi::format_raw;
#endif
        doc.save( writer, "  ", xmlFlags, pugi::encoding_utf8 );

        this->sendPacket( packet );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::sendSettings()
    {
        pugi::xml_document doc;

        pugi::xml_node packetNode = doc.append_child( "Packet" );
        packetNode.append_attribute( "type" ).set_value( "Settings" );

        pugi::xml_node payloadNode = packetNode.append_child( "Payload" );

        SETTINGS_SERVICE()
            ->visitSettings( [&payloadNode]( const ConstString & _name, const SettingInterfacePtr & _settings )
        {
            const FileGroupInterfacePtr & fileGroup = _settings->getFileGroup();
            const FilePath & filePath = _settings->getFilePath();

            Char fullPath[MENGINE_MAX_PATH] = {'\0'};
            if( fileGroup->getFullPath( filePath, fullPath ) == false )
            {
                return;
            }

            pugi::xml_node xml_setting = payloadNode.append_child( "Setting" );

            xml_setting.append_attribute( "name" ).set_value( _name.c_str() );
            xml_setting.append_attribute( "file" ).set_value( fullPath );
        } );

        NodeDebuggerPacket packet;

        MyXMLWriter writer( packet.payload );

#ifdef MENGINE_DEBUG
        const uint32_t xmlFlags = pugi::format_indent;
#else
        const uint32_t xmlFlags = pugi::format_raw;
#endif
        doc.save( writer, "  ", xmlFlags, pugi::encoding_utf8 );

        this->sendPacket( packet );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::sendMemory()
    {
        pugi::xml_document xml_doc;

        pugi::xml_node packetNode = xml_doc.append_child( "Packet" );
        packetNode.append_attribute( "type" ).set_value( "Memory" );

        pugi::xml_node payloadNode = packetNode.append_child( "Payload" );

        size_t allocator_report_total = ALLOCATOR_SERVICE()
            ->get_report_total();

        payloadNode.append_attribute( "Total" ).set_value( allocator_report_total );

        uint32_t availableTextureMemory = RENDER_SYSTEM()
            ->getAvailableTextureMemory();

        payloadNode.append_attribute( "AvailableTextureMemory" ).set_value( availableTextureMemory );

        uint32_t textureMemoryUse = RENDER_SYSTEM()
            ->getTextureMemoryUse();

        payloadNode.append_attribute( "TextureMemoryUse" ).set_value( textureMemoryUse );

        uint32_t textureCount = RENDER_SYSTEM()
            ->getTextureCount();

        payloadNode.append_attribute( "TextureCount" ).set_value( textureCount );

        uint32_t sourcesCount = SOUND_SYSTEM()
            ->getSourcesCount();

        payloadNode.append_attribute( "SoundSourcesCount" ).set_value( sourcesCount );

        uint32_t buffersCount = SOUND_SYSTEM()
            ->getBuffersCount();

        payloadNode.append_attribute( "SoundBuffersCount" ).set_value( buffersCount );

        pugi::xml_node allocatorsNode = payloadNode.append_child( "Allocators" );

        uint32_t allocator_report_count = ALLOCATOR_SERVICE()
            ->get_report_count();

        for( uint32_t index = 0; index != allocator_report_count; ++index )
        {
            const char * report_name;
            size_t report_count = ALLOCATOR_SERVICE()
                ->get_report_info( index, &report_name );

            if( report_name[0] == 0 )
            {
                continue;
            }

            pugi::xml_node allocatorNode = allocatorsNode.append_child( "Allocator" );

            allocatorNode.append_attribute( "Name" ).set_value( report_name );
            allocatorNode.append_attribute( "Count" ).set_value( report_count );
        }

        NodeDebuggerPacket packet;

        MyXMLWriter writer( packet.payload );

#ifdef MENGINE_DEBUG
        const uint32_t xmlFlags = pugi::format_indent;
#else
        const uint32_t xmlFlags = pugi::format_raw;
#endif
        xml_doc.save( writer, "  ", xmlFlags, pugi::encoding_utf8 );

        this->sendPacket( packet );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::sendObjectsLeak()
    {
        uint32_t generation = FACTORY_SERVICE()
            ->getFactoryGeneration();

        if( generation < 2 )
        {
            return;
        }

        pugi::xml_document xml_doc;

        pugi::xml_node packetNode = xml_doc.append_child( "Packet" );
        packetNode.append_attribute( "type" ).set_value( "ObjectsLeak" );

        pugi::xml_node payloadNode = packetNode.append_child( "Payload" );

        payloadNode.append_attribute( "Generation" ).set_value( generation - 1 );

        uint32_t leakcount = 0;

        typedef Vector<DocumentPtr> VectorDocuments;
        typedef Map<String, VectorDocuments> MapObjectLeaks;
        MapObjectLeaks objectLeaks;
        FACTORY_SERVICE()
            ->visitFactoryLeakObjects( generation - 1, [&leakcount, &objectLeaks]( const Factory * _factory, const Factorable * _factorable, const Char * _type, const DocumentPtr & _doc )
        {
            MENGINE_UNUSED( _factory );
            MENGINE_UNUSED( _factorable );

            if( _doc == nullptr )
            {
                return;
            }

            objectLeaks[_type].emplace_back( _doc );

            ++leakcount;
        } );

        pugi::xml_node xml_leaks = payloadNode.append_child( "Leaks" );

        for( auto && [factory, objects] : objectLeaks )
        {
            pugi::xml_node xml_objects = xml_leaks.append_child( "Objects" );

            xml_objects.append_attribute( "Factory" ).set_value( factory.c_str() );

            for( const DocumentPtr & doc : objects )
            {
                pugi::xml_node xml_object = xml_objects.append_child( "Object" );

                xml_object.append_attribute( "File" ).set_value( doc->getFile() );
                xml_object.append_attribute( "Function" ).set_value( doc->getFunction() );
                xml_object.append_attribute( "Line" ).set_value( doc->getLine() );
                xml_object.append_attribute( "Message" ).set_value( MENGINE_DOCUMENT_STR( doc ) );

                DocumentPtr doc_parent = doc->getParent();

                while( doc_parent != nullptr )
                {
                    pugi::xml_node xml_parent = xml_object.append_child( "Parent" );

                    xml_parent.append_attribute( "File" ).set_value( doc_parent->getFile() );
                    xml_parent.append_attribute( "Function" ).set_value( doc_parent->getFunction() );
                    xml_parent.append_attribute( "Line" ).set_value( doc_parent->getLine() );
                    xml_parent.append_attribute( "Message" ).set_value( MENGINE_DOCUMENT_STR( doc_parent ) );

                    doc_parent = doc_parent->getParent();
                }
            }
        }

        NodeDebuggerPacket packet;

        MyXMLWriter writer( packet.payload );

#ifdef MENGINE_DEBUG
        const uint32_t xmlFlags = pugi::format_indent;
#else
        const uint32_t xmlFlags = pugi::format_raw;
#endif
        xml_doc.save( writer, "  ", xmlFlags, pugi::encoding_utf8 );

        this->sendPacket( packet );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::sendNetwork()
    {
        pugi::xml_document xml_doc;

        pugi::xml_node packetNode = xml_doc.append_child( "Packet" );
        packetNode.append_attribute( "type" ).set_value( "Network" );

        pugi::xml_node payloadNode = packetNode.append_child( "Payload" );

        pugi::xml_node network_xml_objects = payloadNode.append_child( "Objects" );

        VectorRequestData data;
        m_networkLogger->getPreparedData( &data );

        for( const RequestData & request : data )
        {
            pugi::xml_node xml_object = network_xml_objects.append_child( "Object" );

            xml_object.append_attribute( "Type" ).set_value( request.type.c_str() );
            xml_object.append_attribute( "Id" ).set_value( request.id );
            xml_object.append_attribute( "Url" ).set_value( request.url.c_str() );
        }

        NodeDebuggerPacket packet;

        MyXMLWriter writer( packet.payload );

#ifdef MENGINE_DEBUG
        const uint32_t xmlFlags = pugi::format_indent;
#else
        const uint32_t xmlFlags = pugi::format_raw;
#endif
        xml_doc.save( writer, "  ", xmlFlags, pugi::encoding_utf8 );

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

        if( typeStr == "Tab" )
        {
            String tab = payloadNode.attribute( "value" ).as_string();

            m_currentTab = tab;

            m_shouldUpdateScene = true;
        }
        else if( typeStr == "Node" )
        {
            pugi::xml_node xmlNode = payloadNode.child( "Node" );

            if( xmlNode )
            {
                this->receiveChangedNode( xmlNode );
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
                    this->stringToPath( valueAttr.value(), &m_selectedNodePath );
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
                    this->receiveGameControlCommand( valueAttr.as_string() );
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::receiveChangedNode( const pugi::xml_node & _xmlNode )
    {
        String pathStr = _xmlNode.attribute( "path" ).value();

        VectorNodePath path;
        this->stringToPath( pathStr, &path );

        NodePtr node = Helper::findUniqueNode( m_scene, path );

        if( node != nullptr )
        {
            Detail::deserializeNodeProp<bool>( "enable", _xmlNode, [node]( bool _value )
            {
                if( _value == node->isEnable() )
                {
                    return;
                }

                if( _value == true )
                {
                    node->enable();
                }
                else
                {
                    node->disable();
                }
            } );

            pugi::xml_node transformationNode = _xmlNode.child( "Transformation" );

            TransformationInterface * transformation = node->getTransformation();

            Detail::deserializeNodeProp<mt::vec3f>( "position", transformationNode, [transformation]( const mt::vec3f & _value )
            {
                transformation->setLocalPosition( _value );
            } );

            Detail::deserializeNodeProp<mt::vec3f>( "origin", transformationNode, [transformation]( const mt::vec3f & _value )
            {
                transformation->setLocalOrigin( _value );
            } );

            Detail::deserializeNodeProp<mt::vec2f>( "skew", transformationNode, [transformation]( const mt::vec2f & _value )
            {
                transformation->setLocalSkew( _value );
            } );

            Detail::deserializeNodeProp<mt::vec3f>( "scale", transformationNode, [transformation]( const mt::vec3f & _value )
            {
                transformation->setLocalScale( _value );
            } );

            Detail::deserializeNodeProp<mt::vec3f>( "orientation", transformationNode, [transformation]( const mt::vec3f & _value )
            {
                transformation->setLocalOrientation( _value );
            } );

            pugi::xml_node renderNode = _xmlNode.child( "Render" );

            if( renderNode )
            {
                RenderInterface * render = node->getRender();

                Detail::deserializeNodeProp<bool>( "hide", renderNode, [render]( bool _value )
                {
                    render->setHide( _value );
                } );

                Detail::deserializeNodeProp<Color>( "local_color", renderNode, [render]( const Color & _value )
                {
                    render->setLocalColor( _value );
                } );

                Detail::deserializeNodeProp<Color>( "personal_color", renderNode, [render]( const Color & _value )
                {
                    render->setPersonalColor( _value );
                } );
            }

            pugi::xml_node animationNode = _xmlNode.child( "Animation" );

            if( animationNode )
            {
                AnimationInterface * animation = node->getAnimation();

                Detail::deserializeNodeProp<bool>( "loop", animationNode, [animation]( bool _value )
                {
                    animation->setLoop( _value );
                } );

                Detail::deserializeNodeProp<bool>( "time", animationNode, [animation]( float _value )
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

                Detail::deserializeNodeProp<float>( "MaxLength", typeNodeTextField, [textField]( float _value )
                {
                    textField->setMaxLength( _value );
                } );

                Detail::deserializeNodeProp<bool>( "Wrap", typeNodeTextField, [textField]( bool _value )
                {
                    textField->setWrap( _value );
                } );

                Detail::deserializeNodeProp<ConstString>( "TextID", typeNodeTextField, [textField]( const ConstString & _value )
                {
                    textField->setTextId( _value );
                } );

                Detail::deserializeNodeProp<ConstString>( "TextAliasEnvironment", typeNodeTextField, [textField]( const ConstString & _value )
                {
                    textField->setTextAliasEnvironment( _value );
                } );

                Detail::deserializeNodeProp<ConstString>( "FontName", typeNodeTextField, [textField]( const ConstString & _value )
                {
                    TextFontInterfacePtr font;
                    
                    if( FONT_SERVICE()
                        ->existFont( _value, &font ) == false )
                    {
                        return;
                    }

                    textField->setFont( font );
                } );

                Detail::deserializeNodeProp<Color>( "FontColor", typeNodeTextField, [textField]( const Color & _value )
                {
                    textField->setFontColor( _value );
                } );

                Detail::deserializeNodeProp<float>( "LineOffset", typeNodeTextField, [textField]( float _value )
                {
                    textField->setLineOffset( _value );
                } );

                Detail::deserializeNodeProp<float>( "CharOffset", typeNodeTextField, [textField]( float _value )
                {
                    textField->setCharOffset( _value );
                } );

                Detail::deserializeNodeProp<float>( "CharScale", typeNodeTextField, [textField]( float _value )
                {
                    textField->setCharScale( _value );
                } );

                Detail::deserializeNodeProp<uint32_t>( "HorizontAlign", typeNodeTextField, [textField]( uint32_t _value )
                {
                    textField->setHorizontAlign( (ETextHorizontAlign)_value );
                } );

                Detail::deserializeNodeProp<uint32_t>( "VerticalAlign", typeNodeTextField, [textField]( uint32_t _value )
                {
                    textField->setVerticalAlign( (ETextVerticalAlign)_value );
                } );

                Detail::deserializeNodeProp<uint32_t>( "MaxCharCount", typeNodeTextField, [textField]( uint32_t _value )
                {
                    textField->setMaxCharCount( _value );
                } );

                Detail::deserializeNodeProp<bool>( "Pixelsnap", typeNodeTextField, [textField]( bool _value )
                {
                    textField->setPixelsnap( _value );
                } );
            }

            pugi::xml_node typeNodeMovie2 = _xmlNode.child( "Type:Movie2" );

            if( typeNodeMovie2 )
            {
                UnknownMovie2Interface * unknownMovie2 = node->getUnknown();

                Detail::deserializeNodeProp<ConstString>( "TextAliasEnvironment", typeNodeMovie2, [unknownMovie2]( const ConstString & _value )
                {
                    unknownMovie2->setTextAliasEnvironment( _value );
                } );

                pugi::xml_node xmlSubCompositions = typeNodeMovie2.child( "SubCompositions" );

                if( xmlSubCompositions )
                {
                    for( pugi::xml_node xmlElement : xmlSubCompositions.children( "Element" ) )
                    {
                        const Char * SubCompositionName = xmlElement.attribute( "SubCompositionName" ).as_string();

                        const Movie2SubCompositionInterfacePtr & subComposition = unknownMovie2->getSubComposition( Helper::stringizeString( SubCompositionName ) );

                        Detail::deserializeNodeProp<bool>( "SubCompositionEnable", xmlElement, [subComposition]( bool _value )
                        {
                            subComposition->setSubCompositionEnable( _value );
                        } );

                        //Detail::serializeNodeProp( _subComposition->getName(), "SubCompositionName", xmlElement );
                        //Detail::serializeNodeProp( _subComposition->getSubCompositionEnable(), "SubCompositionEnable", xmlElement );

                        //AnimationInterface * animation = _subComposition->getAnimation();
                    }
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::receiveGameControlCommand( const String & _command )
    {
        if( _command == "pause" )
        {
            bool alreadyFrozen = APPLICATION_SERVICE()
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
    void NodeDebuggerModule::stringToPath( const String & _str, VectorNodePath * const _path ) const
    {
        if( _str.empty() == false && _str[0] != '-' )
        {
            const Char * ptr = _str.c_str();

            uint32_t uid = 0;
            while( *ptr )
            {
                if( *ptr == MENGINE_PATH_DELIM )
                {
                    _path->push_back( uid );
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
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::notifyChangeArrow( const ArrowPtr & _arrow )
    {
        this->setArrow( _arrow );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::notifyChangeSceneComplete( const ScenePtr & _scene )
    {
        this->setScene( _scene );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::notifyChangeSceneDestroy( const ScenePtr & _scene )
    {
        MENGINE_UNUSED( _scene );

        this->setScene( nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::notifyRemoveSceneDestroy()
    {
        this->setScene( nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::notifyIncrefFactoryGeneration( uint32_t _generator )
    {
        MENGINE_UNUSED( _generator );

        m_shouldUpdateScene = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::setUpdateSceneFlag( bool _flag )
    {
        m_shouldUpdateScene = _flag;
    }
    //////////////////////////////////////////////////////////////////////////
}
