#include "NodeDebugRenderService.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/AllocatorSystemInterface.h"
#include "Interface/UnknownAllocatorDebugReportInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/RenderServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/FactoryServiceInterface.h"
#include "Interface/ScriptProviderServiceInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/FontServiceInterface.h"
#include "Interface/TimerServiceInterface.h"
#include "Interface/ArrowServiceInterface.h"

#include "Plugins/AstralaxPlugin/AstralaxInterface.h"

#include "NodeDebugRenderHelper.h"

#include "Engine/TextDebug.h"

#include "Kernel/Assertion.h"
#include "Kernel/ThreadTask.h"
#include "Kernel/SchedulerHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/BuildMode.h"
#include "Kernel/Stringstream.h"
#include "Kernel/StatisticHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/PrefetcherHelper.h"
#include "Kernel/GlobalInputHandlerHelper.h"
#include "Kernel/Surface.h"

#include <iomanip>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( NodeDebugRenderService, Mengine::NodeDebugRenderService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    NodeDebugRenderService::NodeDebugRenderService()
        : m_fps( 0 )
        , m_showDebugText( 0 )
        , m_globalKeyHandlerF9( INVALID_UNIQUE_ID )
        , m_timerFPS( INVALID_UNIQUE_ID )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    NodeDebugRenderService::~NodeDebugRenderService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeDebugRenderService::_availableService() const
    {
        bool developmentMode = Helper::isDevelopmentMode();

        if( developmentMode == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugRenderService::_dependencyService()
    {
        SERVICE_DEPENDENCY( NodeDebugRenderService, PlayerServiceInterface );
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeDebugRenderService::_initializeService()
    {
        m_globalKeyHandlerF9 = Helper::addGlobalKeyHandler( KC_F9, true, [this]( const InputKeyEvent & _event )
        {
            MENGINE_UNUSED( _event );
            this->toggleDebugText_();
        }, MENGINE_DOCUMENT_FACTORABLE );

        m_timerFPS = TIMER_SERVICE()
            ->addTimer( 1000, [this]( UniqueId _id )
        {
            MENGINE_UNUSED( _id );

            static int64_t old_RenderFrameCount = 0;

            int64_t Statistic_RenderFrameCount = STATISTIC_GET_INTEGER( STATISTIC_RENDER_FRAME_COUNT );

            m_fps = (uint32_t)(Statistic_RenderFrameCount - old_RenderFrameCount);
            old_RenderFrameCount = Statistic_RenderFrameCount;
        }, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugRenderService::_finalizeService()
    {
        m_nodeDebugRenders.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugRenderService::_stopService()
    {
        if( m_globalKeyHandlerF9 != INVALID_UNIQUE_ID )
        {
            Helper::removeGlobalHandler( m_globalKeyHandlerF9 );
            m_globalKeyHandlerF9 = INVALID_UNIQUE_ID;
        }

        if( m_timerFPS != INVALID_UNIQUE_ID )
        {
            TIMER_SERVICE()
                ->removeTimer( m_timerFPS );

            m_timerFPS = INVALID_UNIQUE_ID;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugRenderService::renderDebugArrow( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, bool _external, bool _hide )
    {
        const NodePtr & node = ARROW_SERVICE()
            ->getNode();

        this->renderDebugNode( node, _renderPipeline, _context, _external, _hide );

        const TransformationInterface * transformation = node->getTransformation();

        const mt::mat4f & wm = transformation->getWorldMatrix();

        EArrowType arrowType = ARROW_SERVICE()
            ->getArrowType();

        switch( arrowType )
        {
        case EAT_POINT:
            {
                float radius = 2.5f;

                Helper::nodeDebugRenderCircle( _renderPipeline, _context, wm, radius, 4, 0x8080FFFF, MENGINE_DOCUMENT_FORWARD );
            }break;
        case EAT_RADIUS:
            {
                float radius = ARROW_SERVICE()
                    ->getRadius();

                Helper::nodeDebugRenderCircle( _renderPipeline, _context, wm, radius, 32, 0x8080FFFF, MENGINE_DOCUMENT_FORWARD );
            }break;
        case EAT_POLYGON:
            {
                const Polygon & polygon = ARROW_SERVICE()
                    ->getPolygon();

                Helper::nodeDebugRenderPolygon( _renderPipeline, _context, wm, polygon, 0x8080FFFF, MENGINE_DOCUMENT_FORWARD );
            }break;
        }        
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugRenderService::renderDebugNode( const NodePtr & _node, const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, bool _external, bool _hide )
    {
        if( _node->isActivate() == false )
        {
            return;
        }

        const RenderInterface * selfRender = _node->getRender();

        if( selfRender != nullptr )
        {
            bool selfHide = _hide;

            if( selfRender->isRenderEnable() == false )
            {
                selfHide = true;
            }

            if( selfRender->isHide() == true )
            {
                selfHide = true;
            }

            if( selfRender->isLocalTransparent() == true )
            {
                selfHide = true;
            }

            if( selfRender->isExternalRender() == true && _external == false )
            {
                selfHide = true;
            }

            RenderContext self_context;
            selfRender->mergeRenderContext( _context, &self_context );

            if( selfRender->isLocalHide() == false && selfRender->isPersonalTransparent() == false )
            {
                if( selfHide == false )
                {
                    selfRender->render( _renderPipeline, &self_context );
                }

                const ConstString & type = _node->getType();

                const NodeDebugRenderInterfacePtr & nodeDebugRender = m_nodeDebugRenders.find( type );

                if( nodeDebugRender != nullptr )
                {
                    nodeDebugRender->render( _renderPipeline, &self_context, _node.get(), selfHide );
                }
            }

            const RenderContext * children_context = &self_context;
            _node->foreachChildrenUnslug( [this, &_renderPipeline, children_context, selfHide]( const NodePtr & _child )
            {
                this->renderDebugNode( _child, _renderPipeline, children_context, false, selfHide );
            } );

            if( self_context.target != nullptr )
            {
                const RenderInterfacePtr & targetRender = selfRender->makeTargetRender( &self_context );

                if( targetRender != nullptr )
                {
                    targetRender->render( _renderPipeline, _context );
                }
            }
        }
        else
        {
            const ConstString & type = _node->getType();

            const NodeDebugRenderInterfacePtr & nodeDebugRender = m_nodeDebugRenders.find( type );

            if( nodeDebugRender != nullptr )
            {
                nodeDebugRender->render( _renderPipeline, _context, _node.get(), _hide );
            }

            _node->foreachChildrenUnslug( [this, &_renderPipeline, _context, _hide]( const NodePtr & _child )
            {
                this->renderDebugNode( _child, _renderPipeline, _context, false, _hide );
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugRenderService::addNodeDebugRender( const ConstString & _type, const NodeDebugRenderInterfacePtr & _nodeDebugRender )
    {
        m_nodeDebugRenders.emplace( _type, _nodeDebugRender );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugRenderService::removeNodeDebugRender( const ConstString & _type )
    {
        m_nodeDebugRenders.erase( _type );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugRenderService::updateDebugInfo( const UpdateContext * _context )
    {
        MENGINE_UNUSED( _context );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugRenderService::renderDebugInfo( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context )
    {
        MENGINE_UNUSED( _context );

        bool developmentMode = Helper::isDevelopmentMode();

        if( developmentMode == false )
        {
            return;
        }

        if( m_showDebugText == 0 )
        {
            _renderPipeline->enableDebugFillrateCalcMode( false );
        }

        if( m_showDebugText != 0 )
        {
            Stringstream ss;

            ss << "FPS: " << m_fps << std::endl;

            if( m_showDebugText > 1 )
            {
                _renderPipeline->enableDebugFillrateCalcMode( true );

                const Resolution & contentResolution = APPLICATION_SERVICE()
                    ->getContentResolution();

                double Statistic_RenderFillrate = STATISTIC_GET_DOUBLE( STATISTIC_RENDER_PERFRAME_FILLRATE );

                double sreenfillrate = Statistic_RenderFillrate / double( contentResolution.getWidth() * contentResolution.getHeight() );

                int64_t Statistic_RenderObjects = STATISTIC_GET_INTEGER( STATISTIC_RENDER_PERFRAME_OBJECTS );
                int64_t Statistic_RenderTriangles = STATISTIC_GET_INTEGER( STATISTIC_RENDER_PERFRAME_TRIANGLES );
                int64_t Statistic_DrawIndexPrimitives = STATISTIC_GET_INTEGER( STATISTIC_RENDER_PERFRAME_DRAWINDEXPRIMITIVES );

                ss << "Fillrate " << std::setiosflags( std::ios::fixed ) << std::setprecision( 2 ) << sreenfillrate << " (Object " << Statistic_RenderObjects << " Triangle " << Statistic_RenderTriangles << ")" << std::endl;
                ss << "DIP: " << Statistic_DrawIndexPrimitives << std::endl;

                //ERenderBatchMode mode = RENDER_SERVICE()
                //    ->getBatchMode();

                //ss << "Smart Batch: " << mode << " " << rdi.batch << std::endl;

                uint32_t availableTextureMemory = RENDER_SYSTEM()
                    ->getAvailableTextureMemory();

                ss << "Available Texture Memory: " << (float)availableTextureMemory / (1024.f * 1024.f) << std::endl;

                int64_t textureMemoryUse = STATISTIC_GET_INTEGER( STATISTIC_RENDER_TEXTURE_ALLOC_SIZE );

                ss << "Texture Memory Usage: " << (float)textureMemoryUse / (1024.f * 1024.f) << std::endl;

                int64_t textureCount = STATISTIC_GET_INTEGER( STATISTIC_RENDER_TEXTURE_ALLOC_COUNT );

                ss << "Texture Count: " << textureCount << std::endl;

                if( SERVICE_IS_INITIALIZE( Mengine::AstralaxServiceInterface ) == true )
                {
                    uint32_t particlesCount = ASTRALAX_SERVICE()
                        ->getEmitterCount();

                    ss << "Particles: " << particlesCount << std::endl;
                }
            }

            if( m_showDebugText == 2 )
            {
                uint32_t resourceCount = 0;

                RESOURCE_SERVICE()
                    ->foreachResources( [&resourceCount]( const ResourcePtr & _resource )
                {
                    if( _resource->isCompile() == false )
                    {
                        return;
                    }

                    ++resourceCount;
                } );

                ss << "Resources: " << resourceCount << std::endl;

                uint32_t prefetchCount = 0;

                Helper::foreachPrefetches( [&prefetchCount]( const ThreadTaskPtr & _prefetcher )
                {
                    if( _prefetcher->isComplete() == false )
                    {
                        return;
                    }

                    ++prefetchCount;
                } );

                ss << "Prefetcher " << prefetchCount << std::endl;
            }
            else if( m_showDebugText == 3 )
            {
                //////////////////////////////////////////////////////////////////////////
                class VisitorPlayerFactoryManager
                {
                public:
                    VisitorPlayerFactoryManager( const ConstString & _category )
                        : m_category( _category )
                    {
                    }

                    ~VisitorPlayerFactoryManager()
                    {
                    }

                public:
                    void visit( const PrototypeGeneratorInterfacePtr & _generator )
                    {
                        const ConstString & category = _generator->getCategory();
                        const ConstString & prototype = _generator->getPrototype();

                        if( m_category != category )
                        {
                            return;
                        }

                        uint32_t count = _generator->count();

                        if( count == 0 )
                        {
                            return;
                        }

                        m_scopes[count].emplace_back( prototype );
                    }

                public:
                    String getMsg() const
                    {
                        Stringstream ss;

                        uint32_t iterator = 0;

                        for( MapPybindScope::const_reverse_iterator
                            it = m_scopes.rbegin(),
                            it_end = m_scopes.rend();
                            it != it_end;
                            ++it )
                        {
                            uint32_t c = it->first;
                            const VectorConstString & l = it->second;

                            for( const ConstString & s : l )
                            {
                                ss << "Prototype: " << s.c_str() << " " << c << std::endl;

                                if( ++iterator == 15 )
                                {
                                    break;
                                }
                            }

                            if( iterator == 15 )
                            {
                                break;
                            }
                        }

                        return ss.str();
                    }

                protected:
                    ConstString m_category;

                    typedef Map<uint32_t, VectorConstString> MapPybindScope;
                    MapPybindScope m_scopes;
                };

                VisitorPlayerFactoryManager pfmv_node( Node::getFactorableType() );

                PROTOTYPE_SERVICE()
                    ->foreachGenerators( [&pfmv_node]( const PrototypeGeneratorInterfacePtr & _generator )
                {
                    pfmv_node.visit( _generator );
                } );

                ss << pfmv_node.getMsg() << std::endl;

                VisitorPlayerFactoryManager pfmv_surface( Surface::getFactorableType() );

                PROTOTYPE_SERVICE()
                    ->foreachGenerators( [&pfmv_surface]( const PrototypeGeneratorInterfacePtr & _generator )
                {
                    pfmv_surface.visit( _generator );
                } );

                ss << pfmv_surface.getMsg() << std::endl;
            }
            else if( m_showDebugText == 5 )
            {
                class MyVisitorFactoryService
                    : public Factorable
                {
                public:
                    MyVisitorFactoryService()
                    {
                    }

                    ~MyVisitorFactoryService() override
                    {
                    }

                protected:
                    void operator = ( const MyVisitorFactoryService & ) = delete;

                public:
                    void visit( const FactoryInterface * _factory )
                    {
                        uint32_t count = _factory->getCountObject();

                        if( count == 0 )
                        {
                            return;
                        }

                        ConstString name = _factory->getType();

                        m_scopes[count].emplace_back( name );
                    }

                public:
                    String getMsg() const
                    {
                        Stringstream ss;

                        uint32_t iterator = 0;

                        for( MapPybindScope::const_reverse_iterator
                            it = m_scopes.rbegin(),
                            it_end = m_scopes.rend();
                            it != it_end;
                            ++it )
                        {
                            uint32_t c = it->first;
                            const VectorConstString & l = it->second;

                            for( const ConstString & s : l )
                            {
                                ss << "Factory: " << s.c_str() << " " << c << std::endl;

                                if( ++iterator == 15 )
                                {
                                    break;
                                }
                            }

                            if( iterator == 15 )
                            {
                                break;
                            }
                        }

                        return ss.str();
                    }

                protected:
                    typedef Map<uint32_t, VectorConstString> MapPybindScope;
                    MapPybindScope m_scopes;
                };

                typedef IntrusivePtr<MyVisitorFactoryService> MyVisitorFactoryServicePtr;

                MyVisitorFactoryServicePtr mvcts = Helper::makeFactorableUnique<MyVisitorFactoryService>( MENGINE_DOCUMENT_FACTORABLE );
                FACTORY_SERVICE()
                    ->foreachFactories( [mvcts]( const FactoryInterface * _factory )
                {
                    mvcts->visit( _factory );
                } );

                String msg_python = mvcts->getMsg();
                ss << msg_python << std::endl;
            }
            else if( m_showDebugText == 6 )
            {
                typedef Map<size_t, VectorString> MapPybindScope;
                MapPybindScope scopes;

                UnknownAllocatorDebugReportInterface * debugReport = ALLOCATOR_SYSTEM()
                    ->getUnknown();

                if( debugReport != nullptr )
                {
                    uint32_t allocator_report_count = debugReport->getAllocatorReportCount();

                    for( uint32_t index = 0; index != allocator_report_count; ++index )
                    {
                        const char * report_name;
                        size_t report_count = debugReport->getAllocatorReportInfo( index, &report_name );

                        if( report_name[0] == '\0' )
                        {
                            continue;
                        }

                        scopes[report_count].emplace_back( report_name );
                    }
                }

                Stringstream ss2;

                uint32_t iterator = 0;

                for( MapPybindScope::const_reverse_iterator
                    it = scopes.rbegin(),
                    it_end = scopes.rend();
                    it != it_end;
                    ++it )
                {
                    size_t c = it->first;
                    const VectorString & l = it->second;

                    for( const String & s : l )
                    {
                        ss2 << "Memory: " << s << " " << c << std::endl;

                        if( iterator++ == 15 )
                        {
                            break;
                        }
                    }

                    if( iterator == 15 )
                    {
                        break;
                    }
                }

                String msg_python = ss2.str();
                ss << msg_python << std::endl;
            }

            String text = ss.str();

            const Resolution & resolution = APPLICATION_SERVICE()
                ->getCurrentWindowResolution();

            const Resolution & content = APPLICATION_SERVICE()
                ->getContentResolution();

            mt::vec2f scale;
            content.calcScale( resolution, &scale );

            const FontInterfacePtr & font = FONT_SERVICE()
                ->getDefaultFont();

            Helper::drawTextDebug( _renderPipeline, _context, {100.f, 100.f}, font, Color( 1.f, 0.f, 0.f, 1.f ), MENGINE_DOCUMENT_FORWARD, "%s"
                , text.c_str()
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////     
    void NodeDebugRenderService::toggleDebugText_()
    {
        ++m_showDebugText;
        m_showDebugText %= 7;
    }
    //////////////////////////////////////////////////////////////////////////
}