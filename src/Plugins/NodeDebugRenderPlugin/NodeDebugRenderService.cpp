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
#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/FactoryServiceInterface.h"
#include "Interface/ScriptProviderServiceInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/FontServiceInterface.h"

#include "Plugins/AstralaxPlugin/AstralaxInterface.h"

#include "Engine/TextDebug.h"

#include "Kernel/Assertion.h"
#include "Kernel/ThreadTask.h"
#include "Kernel/SchedulerHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/BuildMode.h"
#include "Kernel/Stringstream.h"
#include "Kernel/StatisticHelper.h"
#include "Kernel/FactorableUnique.h"

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
        const GlobalInputHandlerInterfacePtr & globalHandleSystem = PLAYER_SERVICE()
            ->getGlobalInputHandler();

        m_globalKeyHandlerF9 = globalHandleSystem->addGlobalKeyHandler( KC_F9, [this]( const InputKeyEvent & _event )
        {
            MENGINE_UNUSED( _event );
            this->toggleDebugText_();
        }, MENGINE_DOCUMENT_FACTORABLE );

        m_timerFPS = PLATFORM_SERVICE()
            ->addTimer( 1000.f, [this]( UniqueId _id )
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
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugRenderService::_stopService()
    {
        if( m_globalKeyHandlerF9 != INVALID_UNIQUE_ID )
        {
            const GlobalInputHandlerInterfacePtr & globalHandleSystem = PLAYER_SERVICE()
                ->getGlobalInputHandler();

            globalHandleSystem->removeGlobalHandler( m_globalKeyHandlerF9 );
            m_globalKeyHandlerF9 = INVALID_UNIQUE_ID;
        }

        if( m_timerFPS != INVALID_UNIQUE_ID )
        {
            PLATFORM_SERVICE()
                ->removeTimer( m_timerFPS );

            m_timerFPS = INVALID_UNIQUE_ID;
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

        const RenderPipelineInterfacePtr & renderPipeline = APPLICATION_SERVICE()
            ->getRenderPipeline();

        if( m_showDebugText == 0 )
        {
            renderPipeline->enableDebugFillrateCalcMode( false );
        }

        if( m_showDebugText != 0 )
        {
            Stringstream ss;

            ss << "FPS: " << m_fps << std::endl;

            if( m_showDebugText > 1 )
            {
                renderPipeline->enableDebugFillrateCalcMode( true );

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

                uint32_t textureMemoryUse = RENDER_SYSTEM()
                    ->getTextureMemoryUse();

                ss << "Texture Memory Usage: " << (float)textureMemoryUse / (1024.f*1024.f) << std::endl;

                uint32_t textureCount = RENDER_SYSTEM()
                    ->getTextureCount();

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
                class CompileResourceVisitor
                    : public Visitor
                    , public Factorable
                    , public ConcreteVisitor<Resource>
                {
                public:
                    CompileResourceVisitor()
                        : m_count( 0 )
                    {
                    }

                    ~CompileResourceVisitor() override
                    {
                    }

                public:
                    uint32_t getCount() const
                    {
                        return m_count;
                    }

                protected:
                    void accept( Resource * _resource ) override
                    {
                        if( _resource->isCompile() == false )
                        {
                            return;
                        }

                        ++m_count;
                    }

                protected:
                    uint32_t m_count;
                };

                typedef IntrusivePtr<CompileResourceVisitor> CompileResourceVisitorPtr;

                CompileResourceVisitorPtr crv = Helper::makeFactorableUnique<CompileResourceVisitor>( MENGINE_DOCUMENT_FACTORABLE );

                RESOURCE_SERVICE()
                    ->visitResources( crv );

                ss << "Resources: " << crv->getCount() << std::endl;

                class CompleteThreadTaskVisitor
                    : public Visitor
                    , public Factorable
                    , public ConcreteVisitor<ThreadTask>
                {
                public:
                    CompleteThreadTaskVisitor()
                        : m_count( 0 )
                    {
                    }

                    ~CompleteThreadTaskVisitor()
                    {
                    }

                public:
                    uint32_t getCount() const
                    {
                        return m_count;
                    }

                protected:
                    void accept( ThreadTask * _task ) override 
                    {
                        if( _task->isComplete() == false )
                        {
                            return;
                        }

                        ++m_count;
                    }

                protected:
                    uint32_t m_count;
                };

                typedef IntrusivePtr<CompleteThreadTaskVisitor> CompleteThreadTaskVisitorPtr;

                CompleteThreadTaskVisitorPtr cttv = Helper::makeFactorableUnique<CompleteThreadTaskVisitor>( MENGINE_DOCUMENT_FACTORABLE );

                PREFETCHER_SERVICE()
                    ->visitPrefetches( cttv );

                ss << "Prefetcher " << cttv->getCount() << std::endl;
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

                VisitorPlayerFactoryManager pfmv_node( STRINGIZE_STRING_LOCAL( "Node" ) );

                PROTOTYPE_SERVICE()
                    ->foreachGenerators( [&pfmv_node]( const PrototypeGeneratorInterfacePtr & _generator ) { pfmv_node.visit( _generator ); } );

                ss << pfmv_node.getMsg() << std::endl;

                VisitorPlayerFactoryManager pfmv_surface( STRINGIZE_STRING_LOCAL( "Surface" ) );

                PROTOTYPE_SERVICE()
                    ->foreachGenerators( [&pfmv_surface]( const PrototypeGeneratorInterfacePtr & _generator ) { pfmv_surface.visit( _generator ); } );

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

            float gameViewportAspect;
            Viewport gameViewport;

            APPLICATION_SERVICE()
                ->getGameViewport( &gameViewportAspect, &gameViewport );

            const Resolution & resolution = APPLICATION_SERVICE()
                ->getCurrentResolution();

            const Resolution & content = APPLICATION_SERVICE()
                ->getContentResolution();

            mt::vec2f scale;
            content.calcScale( resolution, &scale );

            const FontInterfacePtr & font = FONT_SERVICE()
                ->getFont( STRINGIZE_STRING_LOCAL( "__CONSOLE_FONT__" ) );

            Helper::drawTextDebug( _renderPipeline, _context, { 100.f, 100.f }, font, Color( 1.f, 0.f, 0.f, 1.f ), MENGINE_DOCUMENT_FORWARD, "%s"
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