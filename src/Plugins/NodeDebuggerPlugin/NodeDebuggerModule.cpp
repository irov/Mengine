#include "NodeDebuggerModule.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/AllocatorSystemInterface.h"
#include "Interface/UnknownAllocatorDebugReportInterface.h"
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
#include "Interface/TextServiceInterface.h"
#include "Interface/FontServiceInterface.h"
#include "Interface/ArchiveServiceInterface.h"
#include "Interface/SceneServiceInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/PickerInterface.h"
#include "Interface/AnimationInterface.h"
#include "Interface/SettingsServiceInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/ArrowServiceInterface.h"
#include "Interface/SoundServiceInterface.h"

#if defined(MENGINE_PLATFORM_WINDOWS)
#   include "Environment/Windows/Win32CreateProcess.h"
#endif

#include "NodeDebuggerSerialization.h"

#include "HotSpotPolygonDebuggerBoundingBox.h"
#include "TextFieldDebuggerBoundingBox.h"

#include "Engine/HotSpotPolygon.h"
#include "Engine/HotSpotSurface.h"
#include "Engine/TextField.h"

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
#include "Kernel/GlobalInputHandlerHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Stringstream.h"
#include "Kernel/Data.h"
#include "Kernel/StringHelper.h"
#include "Kernel/RenderCameraHelper.h"
#include "Kernel/ResourceImageSubstract.h"
#include "Kernel/PixelFormatHelper.h"
#include "Kernel/ResolutionHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/StatisticHelper.h"
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/MixinDebug.h"
#include "Kernel/DebugFileHelper.h"

#include "Config/StdString.h"
#include "Config/StdAlgorithm.h"
#include "Config/StdIterator.h"
#include "Config/DynamicCast.h"
#include "Config/Path.h"

#define NODEDEBUGGERLISTEN_THREAD_NAME "NodeDebuggerListen"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static uint32_t ceilDivideResolution_( uint64_t _value, uint32_t _divider )
        {
            return (uint32_t)((_value + _divider - 1U) / _divider);
        }
        //////////////////////////////////////////////////////////////////////////
        static Resolution makeAspectContentResolution_( const Resolution & _contentResolution, const Resolution & _windowResolution )
        {
            uint32_t contentWidth = _contentResolution.getWidth();
            uint32_t contentHeight = _contentResolution.getHeight();
            uint32_t windowWidth = _windowResolution.getWidth();
            uint32_t windowHeight = _windowResolution.getHeight();

            if( contentWidth == 0U || contentHeight == 0U || windowWidth == 0U || windowHeight == 0U )
            {
                return _contentResolution;
            }

            uint32_t baseResolution = contentWidth < contentHeight ? contentWidth : contentHeight;

            if( windowWidth < windowHeight )
            {
                uint32_t adaptiveHeight = Detail::ceilDivideResolution_( (uint64_t)baseResolution * windowHeight, windowWidth );

                return Resolution( baseResolution, adaptiveHeight );
            }

            uint32_t adaptiveWidth = Detail::ceilDivideResolution_( (uint64_t)baseResolution * windowWidth, windowHeight );

            return Resolution( adaptiveWidth, baseResolution );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    NodeDebuggerModule::NodeDebuggerModule()
        : m_serverState( ENodeDebuggerServerState::Invalid )
        , m_shouldRecreateServer( false )
        , m_shouldUpdateScene( false )
        , m_workerId( INVALID_UNIQUE_ID )
        , m_globalKeyHandlerF2( INVALID_UNIQUE_ID )
        , m_globalKeyHandlerForSendingSelectedNode( INVALID_UNIQUE_ID )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    NodeDebuggerModule::~NodeDebuggerModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeDebuggerModule::_initializeModule()
    {
        VOCABULARY_SET( DebuggerBoundingBoxInterface, STRINGIZE_STRING_LOCAL( "DebuggerBoundingBox" ), HotSpotPolygon::getFactorableType(), Helper::makeFactorableUnique<HotSpotPolygonDebuggerBoundingBox>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( DebuggerBoundingBoxInterface, STRINGIZE_STRING_LOCAL( "DebuggerBoundingBox" ), HotSpotSurface::getFactorableType(), Helper::makeFactorableUnique<HotSpotPolygonDebuggerBoundingBox>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( DebuggerBoundingBoxInterface, STRINGIZE_STRING_LOCAL( "DebuggerBoundingBox" ), TextField::getFactorableType(), Helper::makeFactorableUnique<TextFieldDebuggerBoundingBox>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CHANGE_SCENE_DESTROY, &NodeDebuggerModule::notifyChangeSceneDestroy, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CHANGE_SCENE_COMPLETE, &NodeDebuggerModule::notifyChangeSceneComplete, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_REMOVE_SCENE_DESTROY, &NodeDebuggerModule::notifyRemoveSceneDestroy, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_INCREF_FACTORY_GENERATION, &NodeDebuggerModule::notifyIncrefFactoryGeneration, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_HTTP_REQUEST, &NodeDebuggerModule::notifyHttpRequest, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_HTTP_RESPONSE, &NodeDebuggerModule::notifyHttpResponse, MENGINE_DOCUMENT_FACTORABLE );

#if defined(MENGINE_PLATFORM_WINDOWS)
        UniqueId globalKeyHandlerF2 = Helper::addGlobalKeyHandler( KC_F2, true, []( const InputKeyEvent & )
        {
            uint32_t exitCode;
            Helper::Win32CreateProcessW( L"NodeDebugger.exe", L"ip=127.0.0.1:18790", false, &exitCode );
        }, MENGINE_DOCUMENT_FACTORABLE );

        m_globalKeyHandlerF2 = globalKeyHandlerF2;
#endif

        UniqueId idForSelectedNodeSender = Helper::addGlobalMouseButtonHandler( EMouseButtonCode::MC_LBUTTON, true, [this]( const InputMouseButtonEvent & _event )
        {
            MENGINE_UNUSED( _event );

            if( _event.special.isAlt == false )
            {
                return;
            }

            const ScenePtr & currentScene = SCENE_SERVICE()
                ->getCurrentScene();

            mt::vec2f cursorAdaptScreenPosition;
            Helper::adaptScreenPosition( _event.position.screen, &cursorAdaptScreenPosition );

            mt::vec2f cursorWorldPosition;
            PLAYER_SERVICE()
                ->calcGlobalMouseWorldPosition( cursorAdaptScreenPosition, &cursorWorldPosition );

            m_cursorAdaptScreenPosition = cursorAdaptScreenPosition;
            m_cursorWorldPosition = cursorWorldPosition;

            this->findChildRecursive( currentScene, _event.position.screen );

            this->sendSelectedNode();

            m_selectedNode = nullptr;

        }, MENGINE_DOCUMENT_FACTORABLE );

        m_globalKeyHandlerForSendingSelectedNode = idForSelectedNodeSender;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::_finalizeModule()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "DebuggerBoundingBox" ), HotSpotPolygon::getFactorableType() );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "DebuggerBoundingBox" ), HotSpotSurface::getFactorableType() );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "DebuggerBoundingBox" ), TextField::getFactorableType() );

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_SCENE_DESTROY );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_SCENE_COMPLETE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_REMOVE_SCENE_DESTROY );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_INCREF_FACTORY_GENERATION );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_HTTP_REQUEST );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_HTTP_RESPONSE );

        if( m_threadJob != nullptr )
        {
            m_threadJob->removeWorker( m_workerId );
            m_workerId = INVALID_UNIQUE_ID;

            THREAD_SERVICE()
                ->joinTask( m_threadJob );

            m_threadJob = nullptr;
        }

        THREAD_SERVICE()
            ->destroyThreadProcessor( STRINGIZE_STRING_LOCAL_I( NODEDEBUGGERLISTEN_THREAD_NAME ) );

        m_scene = nullptr;

        if( m_socket != nullptr )
        {
            m_socket->disconnect();
            m_socket = nullptr;
        }

        m_dataMutex = nullptr;

        m_archivator = nullptr;

        if( m_globalKeyHandlerF2 != INVALID_UNIQUE_ID )
        {
            Helper::removeGlobalHandler( m_globalKeyHandlerF2 );
            m_globalKeyHandlerF2 = INVALID_UNIQUE_ID;
        }

        if( m_globalKeyHandlerForSendingSelectedNode != INVALID_UNIQUE_ID )
        {
            Helper::removeGlobalHandler( m_globalKeyHandlerForSendingSelectedNode );
            m_globalKeyHandlerForSendingSelectedNode = INVALID_UNIQUE_ID;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeDebuggerModule::_availableModule() const
    {
        if( SERVICE_IS_INITIALIZE( SocketSystemInterface ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::onThreadWorkerUpdate( UniqueId _id )
    {
        MENGINE_UNUSED( _id );
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeDebuggerModule::onThreadWorkerWork( UniqueId )
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
                        size_t sent;
                        m_socket->send( p.payload.data(), p.payload.size(), &sent );
                    }
                }

                // now check if we received something (100 ms)
                bool haveSomeData = m_socket->waitForData( 100u );
                if( haveSomeData == true )
                {
                    static MENGINE_CONSTEXPR size_t BUFFER_SIZE = 4096;
                    uint8_t buffer[BUFFER_SIZE];

                    size_t bytesReceived = 0;
                    bool clientDisconnected = false;
                    do
                    {
                        if( m_socket->receive( buffer, BUFFER_SIZE, &bytesReceived ) == true )
                        {
                            m_receivedData.insert( m_receivedData.end(), buffer + 0, buffer + bytesReceived );
                        }
                        else
                        {
                            clientDisconnected = true;
                        }
                    } while( !clientDisconnected && bytesReceived == static_cast<int32_t>(BUFFER_SIZE) );

                    if( clientDisconnected == true )
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

                        size_t dataSizeWithHeader = hdr->compressedSize + sizeof( PacketHeader );

                        NodeDebuggerPacket packet;
                        this->uncompressPacket( packet, *hdr, m_receivedData.data() + sizeof( PacketHeader ) );

                        m_dataMutex->lock();
                        m_incomingPackets.emplace_back( packet );
                        m_dataMutex->unlock();

                        // now remove this packet data from the buffer
                        size_t newSize = m_receivedData.size() - dataSizeWithHeader;
                        
                        if( newSize != 0 )
                        {
                            StdString::memmove( m_receivedData.data(), m_receivedData.data() + dataSizeWithHeader, newSize );

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
    void NodeDebuggerModule::onThreadWorkerDone( UniqueId _id )
    {
        MENGINE_UNUSED( _id );

        //Empty
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
                const NodePtr & arrowNode = ARROW_SERVICE()
                    ->getArrowNode();

                this->sendArrow( arrowNode );

                this->sendScene( m_scene );
                this->sendPickerable( m_scene );
                this->sendRenderable( m_scene );
            }

            if( m_currentTab == "sounds" )
            {
                this->sendSounds();
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
    void NodeDebuggerModule::_preUpdate()
    {
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
        mt::box2_insideout( &absorb_bb );

        const ConstString & type = Helper::getFactorableType( _node );

        DebuggerBoundingBoxInterfacePtr boundingBox = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "DebuggerBoundingBox" ), type );

        if( boundingBox != nullptr )
        {
            mt::box2f bbox;
            if( boundingBox->getBoundingBox( _node, &bbox ) == true )
            {
                mt::box2_merge( &absorb_bb, bbox );

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
                    mt::box2_merge( &absorb_bb, *rbbox );

                    successul = true;
                }
            }
        }

        _node->foreachChildren( [&absorb_bb, &successul]( const NodePtr & _child )
        {
            mt::box2f child_bb;
            if( s_absorbBoundingBox( _child, &child_bb ) == true )
            {
                mt::box2_merge( &absorb_bb, child_bb );

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

        if( m_scene == nullptr )
        {
            return;
        }

        NodePtr node = Helper::findUniqueNode( m_scene, m_selectedNodePath );

        if( node == nullptr )
        {
            return;
        }

        if( node->isEnable() == false )
        {
            return;
        }

        if( node->isActivate() == false )
        {
            return;
        }

        RenderContext node_context = *_context;
        Helper::fillNodeRenderContextInheritance( node.get(), &node_context );

        mt::box2f bbox;
        if( s_absorbBoundingBox( node, &bbox ) == false )
        {
            return;
        }

        const RenderCameraInterface * camera = node_context.camera;

        const mt::mat4f & vpminv = camera->getCameraViewProjectionMatrixInv();

        mt::box2f bcrop;
        mt::mul_v2_v2_m4( &bcrop.minimum, mt::vec2f( -1.f, 1.f ), vpminv );
        mt::mul_v2_v2_m4( &bcrop.maximum, mt::vec2f( 1.f, -1.f ), vpminv );

        mt::box2_crop( &bbox, bcrop );

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

        ColorValue_ARGB color = Helper::makeRGBAF( 1.f, 0.f, 1.f, 1.f );

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

        const RenderVertex2D * vertexData = vertices.data();
        uint32_t vertexCount = (uint32_t)vertices.size();
        const RenderIndex * indexData = indices.data();
        uint32_t indexCount = (uint32_t)indices.size();

        _renderPipeline->addRenderObject( &node_context, debugMaterial, nullptr, vertexData, vertexCount, indexData, indexCount, &bbox, EROF_NONE, MENGINE_DOCUMENT_FORWARD );
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeDebuggerModule::privateInit()
    {
        m_shouldRecreateServer = true;

        m_threadJob = THREAD_SERVICE()
            ->createJob( 50u, MENGINE_DOCUMENT_FACTORABLE );

        if( THREAD_SERVICE()
            ->createThreadProcessor( STRINGIZE_STRING_LOCAL_I( NODEDEBUGGERLISTEN_THREAD_NAME ), MENGINE_THREAD_DESCRIPTION( "MNGNodeDebugger" ), ETP_NORMAL, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( THREAD_SERVICE()
            ->addTask( STRINGIZE_STRING_LOCAL_I( NODEDEBUGGERLISTEN_THREAD_NAME ), m_threadJob, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        m_dataMutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        UniqueId workerId = m_threadJob->addWorker( ThreadWorkerInterfacePtr( this ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION( workerId != INVALID_UNIQUE_ID, "can't add worker" );

        m_workerId = workerId;

        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( archivator, "can't get archivator" );

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
            Data compressedPayload( maxCompressedSize );
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
            StdString::memcpy( _packet.payload.data(), _receivedData, _hdr.compressedSize );
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
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::sendArrow( const NodePtr & _arrow )
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

#if defined(MENGINE_DEBUG)
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

#if defined(MENGINE_DEBUG)
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

#if defined(MENGINE_DEBUG)
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

#if defined(MENGINE_DEBUG)
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
            ->foreachSettings( [&payloadNode]( const ConstString & _name, const SettingInterfacePtr & _setting )
        {
            const ContentInterfacePtr & content = _setting->getContent();

            const FilePath & filePath = content->getFilePath();

            pugi::xml_node xml_setting = payloadNode.append_child( "Setting" );

            xml_setting.append_attribute( "name" ).set_value( _name.c_str() );
            xml_setting.append_attribute( "file" ).set_value( filePath.c_str() );

            _setting->foreachKeys( [&xml_setting, _setting]( const Char * _key )
            {
                pugi::xml_node xml_key = xml_setting.append_child( "Key" );

                xml_key.append_attribute( "name" ).set_value( _key );

                ESettingType type = _setting->getValueType( _key );

                switch( type )
                {
                case EST_NONE:
                    return;
                case EST_BOOL:
                    {
                        xml_key.append_attribute( "type" ).set_value( "bool" );

                        bool value = _setting->getValueBoolean( _key, false );

                        xml_key.append_attribute( "value" ).set_value( value );
                    }break;
                case EST_INTEGER:
                    {
                        xml_key.append_attribute( "type" ).set_value( "int" );

                        int64_t value = _setting->getValueInteger( _key, MENGINE_INT64_C( 0 ) );

                        xml_key.append_attribute( "value" ).set_value( value );
                    }break;
                case EST_REAL:
                    {
                        xml_key.append_attribute( "type" ).set_value( "real" );

                        double value = _setting->getValueFloat( _key, 0.0 );

                        xml_key.append_attribute( "value" ).set_value( value );
                    }break;
                case EST_STRING:
                    {
                        xml_key.append_attribute( "type" ).set_value( "string" );

                        const Char * value = _setting->getValueString( _key, "" );

                        xml_key.append_attribute( "value" ).set_value( value );
                    }break;
                case EST_VEC2F:
                    {
                        xml_key.append_attribute( "type" ).set_value( "vec2f" );

                        mt::vec2f value = _setting->getValueVec2f( _key, mt::vec2f::identity() );

                        Char value_str[256 + 1] = {'\0'};
                        Helper::stringalized( value, value_str, 256 );

                        xml_key.append_attribute( "value" ).set_value( value_str );
                    }break;
                case EST_VEC3F:
                    {
                        xml_key.append_attribute( "type" ).set_value( "vec3f" );

                        mt::vec3f value = _setting->getValueVec3f( _key, mt::vec3f::identity() );

                        Char value_str[256 + 1] = {'\0'};
                        Helper::stringalized( value, value_str, 256 );

                        xml_key.append_attribute( "value" ).set_value( value_str );
                    }break;
                case EST_COLOR:
                    {
                        xml_key.append_attribute( "type" ).set_value( "color" );

                        Color value = _setting->getValueColor( _key, Color::identity() );

                        Char value_str[256 + 1] = {'\0'};
                        Helper::stringalized( value, value_str, 256 );

                        xml_key.append_attribute( "value" ).set_value( value_str );
                    }break;
                }
            } );
        } );

        NodeDebuggerPacket packet;

        MyXMLWriter writer( packet.payload );

#if defined(MENGINE_DEBUG)
        const uint32_t xmlFlags = pugi::format_indent;
#else
        const uint32_t xmlFlags = pugi::format_raw;
#endif

        doc.save( writer, "  ", xmlFlags, pugi::encoding_utf8 );

        this->sendPacket( packet );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::sendSounds()
    {
        pugi::xml_document doc;

        pugi::xml_node packetNode = doc.append_child( "Packet" );
        packetNode.append_attribute( "type" ).set_value( "Sounds" );

        pugi::xml_node payloadNode = packetNode.append_child( "Payload" );

        SOUND_SERVICE()
            ->foreachSoundIdentities( [&payloadNode]( const SoundIdentityInterfacePtr & _identity )
        {
            pugi::xml_node xml_sound = payloadNode.append_child( "Sound" );

            UniqueId id = _identity->getUniqueIdentity();

            bool streamable = _identity->getStreamable();
            bool loop = _identity->getLoop();
            bool turn = _identity->getTurn();
            ESoundSourceCategory category = _identity->getCategory();
            ESoundSourceState state = _identity->getState();

            float timeLeft = _identity->getTimeLeft();

            const MixerMultiplicativeInterfacePtr & mixer = _identity->getMixerVolume();

            float mixer_volume = mixer->mixValue();

            xml_sound.append_attribute( "id" ).set_value( id );
            xml_sound.append_attribute( "streamable" ).set_value( streamable );
            xml_sound.append_attribute( "loop" ).set_value( loop );
            xml_sound.append_attribute( "turn" ).set_value( turn );
            xml_sound.append_attribute( "category" ).set_value( category );
            xml_sound.append_attribute( "state" ).set_value( state );
            xml_sound.append_attribute( "time_left" ).set_value( timeLeft );
            xml_sound.append_attribute( "mixer_volume" ).set_value( mixer_volume );

            const SoundSourceInterfacePtr & source = _identity->getSoundSource();
            const SoundBufferInterfacePtr & buffer = source->getSoundBuffer();
            const SoundDecoderInterfacePtr & decoder = buffer->getDecoder();
            const InputStreamInterfacePtr & stream = decoder->getStream();

            xml_sound.append_attribute( "file" ).set_value( Helper::getDebugFullPath( stream ).c_str() );
        } );

        NodeDebuggerPacket packet;

        MyXMLWriter writer( packet.payload );

#if defined(MENGINE_DEBUG)
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

        int64_t Statistic_AllocatorSize = STATISTIC_GET_INTEGER( STATISTIC_ALLOCATOR_SIZE );

        payloadNode.append_attribute( "Total" ).set_value( Statistic_AllocatorSize );

        uint32_t availableTextureMemory = RENDER_SYSTEM()
            ->getAvailableTextureMemory();

        payloadNode.append_attribute( "AvailableTextureMemory" ).set_value( availableTextureMemory );

        int64_t textureMemoryUse = STATISTIC_GET_INTEGER( STATISTIC_RENDER_TEXTURE_ALLOC_SIZE );

        payloadNode.append_attribute( "TextureMemoryUse" ).set_value( textureMemoryUse );

        int64_t textureCount = STATISTIC_GET_INTEGER( STATISTIC_RENDER_TEXTURE_ALLOC_COUNT );

        payloadNode.append_attribute( "TextureCount" ).set_value( textureCount );

        int64_t soundSourcesCount = STATISTIC_GET_INTEGER( STATISTIC_SOUND_SOURCE_COUNT );

        payloadNode.append_attribute( "SoundSourcesCount" ).set_value( soundSourcesCount );

        int64_t soundBuffersCount = STATISTIC_GET_INTEGER( STATISTIC_SOUND_BUFFER_COUNT );

        payloadNode.append_attribute( "SoundBuffersCount" ).set_value( soundBuffersCount );

        UnknownAllocatorDebugReportInterface * debugReport = ALLOCATOR_SYSTEM()
            ->getUnknown();

        if( debugReport != nullptr )
        {
            pugi::xml_node allocatorsNode = payloadNode.append_child( "Allocators" );

            uint32_t allocator_report_count = debugReport->getAllocatorReportCount();

            for( uint32_t index = 0; index != allocator_report_count; ++index )
            {
                const Char * report_name;
                size_t report_count = debugReport->getAllocatorReportInfo( index, &report_name );

                if( report_name[0] == '\0' )
                {
                    continue;
                }

                pugi::xml_node allocatorNode = allocatorsNode.append_child( "Allocator" );

                allocatorNode.append_attribute( "Name" ).set_value( report_name );
                allocatorNode.append_attribute( "Count" ).set_value( report_count );
            }
        }

        NodeDebuggerPacket packet;

        MyXMLWriter writer( packet.payload );

#if defined(MENGINE_DEBUG)
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
#if defined(MENGINE_DEBUG_FACTORY_ENABLE)
        uint32_t generation = FACTORY_SERVICE()
            ->debugFactoryGetGeneration();

        if( generation < 2 )
        {
            return;
        }

        pugi::xml_document xml_doc;

        pugi::xml_node packetNode = xml_doc.append_child( "Packet" );
        packetNode.append_attribute( "type" ).set_value( "ObjectsLeak" );

        pugi::xml_node payloadNode = packetNode.append_child( "Payload" );

        payloadNode.append_attribute( "Generation" ).set_value( generation - 1 );

#if defined(MENGINE_DOCUMENT_ENABLE)
        typedef Vector<DocumentInterfacePtr> VectorDocuments;
        typedef Map<const FactoryInterface *, VectorDocuments> MapObjectLeaks;
        MapObjectLeaks objectLeaks;
        FACTORY_SERVICE()
            ->debugFactoryForeachLeakObjects( generation - 1, [&objectLeaks]( const FactoryInterface * _factory, const Factorable * _factorable )
        {
            MENGINE_UNUSED( _factory );
            MENGINE_UNUSED( _factorable );

            const DocumentInterfacePtr & doc = _factorable->getDocument();

            if( doc == nullptr )
            {
                return;
            }

            objectLeaks[_factory].emplace_back( doc );
        } );

        pugi::xml_node xml_leaks = payloadNode.append_child( "Leaks" );

        for( auto && [factory, objects] : objectLeaks )
        {
            const ConstString & factoryType = factory->getFactoryType();

            pugi::xml_node xml_objects = xml_leaks.append_child( "Objects" );

            xml_objects.append_attribute( "Factory" ).set_value( factoryType.c_str() );

            for( const DocumentInterfacePtr & doc : objects )
            {
                pugi::xml_node xml_object = xml_objects.append_child( "Object" );

                xml_object.append_attribute( "File" ).set_value( doc->getFile() );
                xml_object.append_attribute( "Function" ).set_value( doc->getFunction() );
                xml_object.append_attribute( "Line" ).set_value( doc->getLine() );
                xml_object.append_attribute( "Message" ).set_value( MENGINE_DOCUMENT_STR( doc ) );

                DocumentInterfacePtr doc_parent = doc->getParent();

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
#endif

        NodeDebuggerPacket packet;

        MyXMLWriter writer( packet.payload );

#if defined(MENGINE_DEBUG)
        const uint32_t xmlFlags = pugi::format_indent;
#else
        const uint32_t xmlFlags = pugi::format_raw;
#endif
        xml_doc.save( writer, "  ", xmlFlags, pugi::encoding_utf8 );

        this->sendPacket( packet );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::sendNetwork()
    {
        pugi::xml_document xml_doc;

        pugi::xml_node packetNode = xml_doc.append_child( "Packet" );
        packetNode.append_attribute( "type" ).set_value( "Network" );

        pugi::xml_node payloadNode = packetNode.append_child( "Payload" );

        pugi::xml_node network_xml_objects = payloadNode.append_child( "Objects" );

        this->foreachRequestData( [&network_xml_objects]( const NodeDebuggerRequestData & _data )
        {
            pugi::xml_node xml_object = network_xml_objects.append_child( "Object" );

            xml_object.append_attribute( "Type" ).set_value( _data.type.c_str() );
            xml_object.append_attribute( "Id" ).set_value( _data.id );
            xml_object.append_attribute( "Url" ).set_value( _data.url.c_str() );
        } );

        NodeDebuggerPacket packet;

        MyXMLWriter writer( packet.payload );

#if defined(MENGINE_DEBUG)
        const uint32_t xmlFlags = pugi::format_indent;
#else
        const uint32_t xmlFlags = pugi::format_raw;
#endif
        xml_doc.save( writer, "  ", xmlFlags, pugi::encoding_utf8 );

        this->sendPacket( packet );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::sendSelectedNode()
    {
        pugi::xml_document xml_doc;
        pugi::xml_node packetNode = xml_doc.append_child( "Packet" );
        packetNode.append_attribute( "type" ).set_value( "SelectedNode" );

        pugi::xml_node payloadNode = packetNode.append_child( "Payload" );

        pugi::xml_node xmlNode = payloadNode.append_child( "Node" );

        if( m_selectedNode == nullptr )
        {
            return;
        }

        const ConstString & selectedNodeName = m_selectedNode->getName();
        xmlNode.append_attribute( "SelectedNodeName" ).set_value( selectedNodeName.c_str() );

        UniqueId selectedNodeId = m_selectedNode->getUniqueIdentity();
        xmlNode.append_attribute( "SelectedNodeId" ).set_value( selectedNodeId );

        VectorNodePath pathToRoot;
        Helper::findPathToRootFromParent( m_scene, m_selectedNode, &pathToRoot );

        String pathStr;
        this->pathToString( pathToRoot, &pathStr );

        xmlNode.append_attribute( "PathToRoot" ).set_value( pathStr.c_str() );

        NodeDebuggerPacket packet;

        MyXMLWriter writer( packet.payload );

#if defined(MENGINE_DEBUG)
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
        else if( typeStr == "Resolutions" )
        {
            pugi::xml_node xmlNode = payloadNode.child( "Resolution" );
            if( xmlNode )
            {
                pugi::xml_attribute widthAttr = xmlNode.attribute( "width" );
                pugi::xml_attribute heightAttr = xmlNode.attribute( "height" );

                uint32_t width = widthAttr.as_uint();
                uint32_t height = heightAttr.as_uint();

                this->receiveResolutins( width, height );
            }
        }
        else if( typeStr == "Settings" )
        {
            pugi::xml_node xmlNode = payloadNode.child( "Setting" );
            if( xmlNode )
            {
                pugi::xml_attribute nameAttr = xmlNode.attribute( "name" );
                pugi::xml_attribute keyAttr = xmlNode.attribute( "key" );
                pugi::xml_attribute valueAttr = xmlNode.attribute( "value" );

                const Char * name = nameAttr.as_string();
                const Char * key = keyAttr.as_string();
                const Char * value = valueAttr.as_string();

                this->receiveSetting( name, key, value );
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

        if( node == nullptr )
        {
            const NodePtr & arrowNode = ARROW_SERVICE()
                ->getArrowNode();

            if( arrowNode != nullptr )
            {
                node = Helper::findUniqueNode( arrowNode, path );
            }
        }

        this->applyNodeProperties( node, _xmlNode );
        m_shouldUpdateScene = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::receiveGameControlCommand( const String & _command )
    {
        if( _command == "pause" )
        {
            bool alreadyFrozen = APPLICATION_SERVICE()
                ->isUpdateFrozen();

            APPLICATION_SERVICE()
                ->setUpdateFreeze( NodeDebuggerModule::getFactorableType(), !alreadyFrozen );
        }
        else if( _command == "scene" )
        {
            m_shouldUpdateScene = true;
        }
        else if( _command == "mute" )
        {
            bool alredyMute = SOUND_SERVICE()
                ->getMute( STRINGIZE_STRING_LOCAL( "NodeDebugger" ) );

            SOUND_SERVICE()
                ->setMute( STRINGIZE_STRING_LOCAL( "NodeDebugger" ), !alredyMute );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::receiveResolutins( uint32_t _width, uint32_t _height )
    {
        if( _width == 0U || _height == 0U )
        {
            return;
        }

        Resolution newResolution( _width, _height );

        const Resolution & contentResolution = APPLICATION_SERVICE()
            ->getContentResolution();

        Resolution newContentResolution = Detail::makeAspectContentResolution_( contentResolution, newResolution );

        APPLICATION_SERVICE()
            ->setContentResolution( newContentResolution );

        APPLICATION_SERVICE()
            ->setWindowResolution( newResolution );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::receiveSetting( const Char * _setting, const Char * _key, const Char * _value )
    {
        ConstString setting_cstr = Helper::stringizeString( _setting );

        SettingInterfacePtr setting = SETTINGS_SERVICE()
            ->getSetting( setting_cstr );

        if( setting == nullptr )
        {
            return;
        }

        ESettingType type = setting->getValueType( _key );

        switch( type )
        {
        case EST_NONE:
            return;
        case EST_BOOL:
            {
                bool value;
                Helper::stringalized( _value, &value );

                setting->setValueBoolean( _key, value );
            }break;
        case EST_INTEGER:
            {
                int64_t value;
                Helper::stringalized( _value, &value );

                setting->setValueInteger( _key, value );
            }break;
        case EST_REAL:
            {
                double value;
                Helper::stringalized( _value, &value );

                setting->setValueFloat( _key, value );
            }break;
        case EST_STRING:
            {
                setting->setValueString( _key, _value );
            }break;
        case EST_VEC2F:
            {
                mt::vec2f value;
                Helper::stringalized( _value, &value );

                setting->setValueVec2f( _key, value );
            }break;
        case EST_VEC3F:
            {
                mt::vec3f value;
                Helper::stringalized( _value, &value );

                setting->setValueVec3f( _key, value );
            }break;
        case EST_COLOR:
            {
                Color value;
                Helper::stringalized( _value, &value );

                setting->setValueColor( _key, value );
            }break;
        }

        SETTINGS_SERVICE()
            ->saveSetting( setting_cstr, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_NOTIFY( NOTIFICATOR_SETTING_CHANGE, setting, _key );
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
                if( *ptr == MENGINE_PATH_FORWARDSLASH )
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
    void NodeDebuggerModule::pathToString( const VectorNodePath & _path, String * const _outStr ) const
    {
        Stringstream stream;
        StdAlgorithm::copy( _path.begin(), _path.end(), std::ostream_iterator<UniqueId>( stream, "/" ) );

        *_outStr = stream.str();
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
    void NodeDebuggerModule::notifyHttpRequest( HttpRequestId _id, const URLString & _url )
    {
        NodeDebuggerRequestData requestData;

        requestData.id = _id;
        requestData.url = _url;
        requestData.type = STRINGIZE_STRING_LOCAL( "Request" );

        m_requestDatas.push_back( requestData );

        this->setUpdateSceneFlag( true );

        this->clearRequestDatas_();
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::notifyHttpResponse( const HttpResponseInterfacePtr & _response )
    {
        NodeDebuggerRequestData responseData;

        responseData.id = _response->getRequest()->getRequestId();
        responseData.url = _response->getRequest()->getURL();
        responseData.type = STRINGIZE_STRING_LOCAL( "Response" );

        m_requestDatas.push_back( responseData );

        this->setUpdateSceneFlag( true );

        this->clearRequestDatas_();
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::foreachRequestData( const LambdaNodeDebuggerRequestData & _lambda )
    {
        for( const NodeDebuggerRequestData & data : m_requestDatas )
        {
            _lambda( data );
        };
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::clearRequestDatas_()
    {
        if( m_requestDatas.size() <= 128 )
        {
            return;
        }

        m_requestDatas.pop_front();
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::setUpdateSceneFlag( bool _flag )
    {
        m_shouldUpdateScene = _flag;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::findChildRecursive( const NodePtr & _currentNode, const mt::vec2f & _point )
    {
        const RenderContext * renderContext = PLAYER_SERVICE()
            ->getRenderContext();

        bool result = _currentNode->foreachChildrenReverseBreak( [this, renderContext, _point]( const NodePtr & _child ) -> bool
        {
            if( m_selectedNode != nullptr )
            {
                return true;
            }

            if( _child->isEnable() == false )
            {
                return true;
            }

            this->findChildRecursive( _child, _point );

            if( m_selectedNode != nullptr )
            {
                return true;
            }

            const PickerInterface * picker = _child->getPicker();

            if( picker != nullptr )
            {
                if( picker->pick( m_cursorAdaptScreenPosition, renderContext ) == true )
                {
                    m_selectedNode = _child;

                    return false;
                }

                return true;
            }

            const ConstString & type = Helper::getFactorableType( _child );

            DebuggerBoundingBoxInterfacePtr boundingBoxInterfacePtr = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "DebuggerBoundingBox" ), type );

            if( boundingBoxInterfacePtr != nullptr )
            {
                mt::box2f bbox;
                if( boundingBoxInterfacePtr->getBoundingBox( _child, &bbox ) == true )
                {
                    if( mt::box2_intersect( bbox, m_cursorWorldPosition ) == true && mt::box2_is_infinity( bbox ) == false )
                    {
                        m_selectedNode = _child;

                        return false;
                    }
                }

                return true;
            }

            RenderInterface * render = _child->getRender();

            if( render == nullptr )
            {
                return true;
            }

            if( render->isRenderEnable() == false )
            {
                return true;
            }

            if( render->isHide() == true )
            {
                return true;
            }

            if( render->isLocalTransparent() == true )
            {
                return true;
            }

            const mt::box2f * rbb = render->getBoundingBox();

            if( rbb == nullptr )
            {
                return true;
            }

            if( mt::box2_is_infinity( *rbb ) == true )
            {
                return true;
            }

            if( mt::box2_intersect( *rbb, m_cursorWorldPosition ) == false )
            {
                return true;
            }

            ShapePtr shape = ShapePtr::dynamic_from( _child );

            if( shape != nullptr )
            {
                if( this->checkHit( shape, _point ) == false )
                {
                    return false;
                }
            }

            m_selectedNode = _child;

            return true;
        } );

        MENGINE_UNUSED( result );
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeDebuggerModule::checkHit( const ShapePtr & _currentNode, const mt::vec2f & _point )
    {
        const SurfacePtr & surface = _currentNode->getSurface();

        if( surface == nullptr )
        {
            return false;
        }

        const RenderMaterialInterfacePtr & renderMaterial = surface->getMaterial();

        uint32_t textureCount = renderMaterial->getTextureCount();

        if( textureCount == 0 )
        {
            return false;
        }

        const RenderTextureInterfacePtr & renderTextureInterface = renderMaterial->getTexture( 0 );

        const RenderImageInterfacePtr & renderImage = renderTextureInterface->getImage();

        const RenderImageProviderInterfacePtr & renderImageProviderInterface = renderImage->getRenderImageProvider();

        RenderImageLoaderInterfacePtr renderImageLoader = renderImageProviderInterface->getLoader( MENGINE_DOCUMENT_FACTORABLE );

        RenderImageDesc imageDesc;
        renderImageLoader->getImageDesc( &imageDesc );

        mt::box2f bb_screen;
        this->getScreenBoundingBox( _currentNode, imageDesc, &bb_screen );

        if( mt::box2_intersect( bb_screen, _point ) == false && mt::box2_is_infinity( bb_screen ) == true )
        {
            return false;
        }

        EPixelFormat hwPixelFormat = renderImage->getHWPixelFormat();

        uint32_t hwPixelChannels = Helper::getPixelFormatChannels( hwPixelFormat );

        if( hwPixelChannels != 4 )
        {
            return true;
        }

        const mt::uv4f & uv = surface->getUV( 0 );

        if( this->checkIsTransparencePoint( _currentNode, _point, renderImageLoader, renderTextureInterface, imageDesc, uv ) == true )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeDebuggerModule::checkIsTransparencePoint( const ShapePtr & _currentNode
        , const mt::vec2f & _point
        , const RenderImageLoaderInterfacePtr & _imageLoader
        , const RenderTextureInterfacePtr & _renderTexture
        , const RenderImageDesc & _imageDesc
        , const mt::uv4f & _uv )
    {
        MENGINE_UNUSED( _renderTexture );

        const RenderResolutionInterfacePtr & renderResolution = PLAYER_SERVICE()
            ->getRenderResolution();

        const RenderCameraInterfacePtr & renderCamera = PLAYER_SERVICE()
            ->getRenderCamera();

        const RenderViewportInterfacePtr & renderViewport = PLAYER_SERVICE()
            ->getRenderViewport();

        const mt::mat4f & vpm_inv = renderCamera->getCameraViewProjectionMatrixInv();
        const Viewport & vp = renderViewport->getViewportWM();

        mt::vec2f point_vp;
        renderResolution->fromScreenToContentPosition( _point, &point_vp );

        point_vp -= vp.begin;

        mt::vec2f size = vp.size();

        if( size.x < mt::constant::eps || size.y < mt::constant::eps )
        {
            return true;
        }

        point_vp /= size;

        mt::vec2f point_norm;
        point_norm.x = point_vp.x * 2.f - 1.f;
        point_norm.y = 1.f - point_vp.y * 2.f;

        mt::vec2f pointIn1;
        mt::mul_v2_v2_m4( &pointIn1, point_norm, vpm_inv );

        TransformationInterface * currentNodeTransformation = _currentNode->getTransformation();
        const mt::mat4f & wm = currentNodeTransformation->getWorldMatrix();

        mt::mat4f invWM;
        mt::inv_m4_m4( &invWM, wm );

        mt::vec2f pointIn2;
        mt::mul_v2_v2_m4( &pointIn2, pointIn1, invWM );

        if( pointIn2.x < 0.f || pointIn2.y < 0.f )
        {
            return true;
        }

        MemoryInterfacePtr memory = _imageLoader->getMemory( 0, MENGINE_DOCUMENT_FACTORABLE );

        uint8_t * alphaBufferMemory = memory->getBuffer();

        const RenderImageInterfacePtr & renderImage = _renderTexture->getImage();

        uint32_t renderImageWidth = renderImage->getHWWidth();
        uint32_t renderImageHeight = renderImage->getHWHeight();

        mt::vec2f firstPoint;
        firstPoint.x = _uv.p0.x * renderImageWidth;
        firstPoint.y = _uv.p0.y * renderImageHeight;

        uint32_t fuulYdistance = static_cast<uint32_t>(firstPoint.y + pointIn2.y);

        uint32_t alphaIndex = fuulYdistance * _imageDesc.width + static_cast<uint32_t>(pointIn2.x + firstPoint.x);

        alphaIndex *= 4;

        uint8_t alpha = alphaBufferMemory[alphaIndex + 3];

        uint8_t minAlpha = (uint8_t)(0.f * 255.f);

        if( alpha == minAlpha )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::getScreenBoundingBox( const ShapePtr & _node, const RenderImageDesc & _imageDesc, mt::box2f * const _bb ) const
    {
        mt::box2f boundingBox;
        this->getWorldBoundingBox( _node, _imageDesc, &boundingBox );

        const NodePtr & node = ARROW_SERVICE()
            ->getArrowNode();

        RenderInterface * render = node->getRender();

        RenderContext context;
        render->makeRenderContext( &context );

        mt::box2f bb_screen;
        Helper::worldToScreenBox( &context, boundingBox, &bb_screen );

        *_bb = bb_screen;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerModule::getWorldBoundingBox( const ShapePtr & _node, const RenderImageDesc & _imageDesc, mt::box2f * const _bb ) const
    {
        TransformationInterface * currentNodeTransformation = _node->getTransformation();
        const mt::mat4f & worldMatrix = currentNodeTransformation->getWorldMatrix();

        float hs_width = static_cast<float>(_imageDesc.width);
        float hs_height = static_cast<float>(_imageDesc.height);

        mt::vec2f minimal( 0.f, 0.f );
        mt::vec2f maximal( hs_width, hs_height );

        mt::vec2f minimal_wm;
        mt::mul_v2_v2_m4( &minimal_wm, minimal, worldMatrix );

        mt::vec2f maximal_wm;
        mt::mul_v2_v2_m4( &maximal_wm, maximal, worldMatrix );

        mt::box2f bb;
        mt::box2_set_from_two_point( &bb, minimal_wm, maximal_wm );

        *_bb = bb;
    }
    //////////////////////////////////////////////////////////////////////////
}
