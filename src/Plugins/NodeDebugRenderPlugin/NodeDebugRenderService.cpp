#include "NodeDebugRenderService.h"

#include "Interface/OptionsServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/RenderServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/FactoryServiceInterface.h"
#include "Interface/ScriptProviderServiceInterface.h"

#include "Plugins/AstralaxParticlePlugin/AstralaxInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/ThreadTask.h"
#include "Kernel/SchedulerHelper.h"
#include "Kernel/Document.h"

#include "Config/Stringstream.h"

#include "stdex/allocator_report.h"

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
        , m_globalKeyHandlerF9( 0 )
        , m_schedulerFPS( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    NodeDebugRenderService::~NodeDebugRenderService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugRenderService::_dependencyService()
    {
        SERVICE_DEPENDENCY( NodeDebugRenderService, PlayerServiceInterface );
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeDebugRenderService::_initializeService()
    {
        //bool developmentMode = HAS_OPTION( "dev" );

        //if( developmentMode == true )
        //{
        //    const GlobalInputHandlerInterfacePtr & globalHandleSystem = PLAYER_SERVICE()
        //        ->getGlobalInputHandler();

        //    m_globalKeyHandlerF9 = globalHandleSystem->addGlobalKeyHandler( KC_F9, [this]( const InputKeyEvent & _event )
        //    {
        //        MENGINE_UNUSED( _event );
        //        this->toggleDebugText_();
        //    }, "NodeDebugRenderService" );

        //    m_debugText = PROTOTYPE_SERVICE()
        //        ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), "TextField"_c, MENGINE_DOCUMENT_FUNCTION );

        //    m_debugText->setFontName( "__CONSOLE_FONT__"_c );
        //    m_debugText->setTextID( "__ID_TEXT_CONSOLE"_c );
        //    m_debugText->setLocalColor( Color( 1.0, 0.0, 0.0, 1.0 ) );
        //    m_debugText->enable();

        //    const SchedulerInterfacePtr & scheduler = PLAYER_SERVICE()
        //        ->getGlobalScheduler();

        //    m_schedulerFPS = Helper::schedulerTiming( scheduler
        //        , []( uint32_t, uint32_t ) { return 1000.f; }
        //        , [this]( uint32_t, uint32_t, float )
        //    {
        //        const RenderServiceDebugInfo & debugInfo = RENDER_SERVICE()
        //            ->getDebugInfo();

        //        m_fps = debugInfo.frameCount;

        //        RENDER_SERVICE()
        //            ->resetFrameCount();
        //    }, nullptr );
        //}

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugRenderService::_finalizeService()
    {
        //bool developmentMode = HAS_OPTION( "dev" );

        //if( developmentMode == true )
        //{
        //    const GlobalInputHandlerInterfacePtr & globalHandleSystem = PLAYER_SERVICE()
        //        ->getGlobalInputHandler();

        //    globalHandleSystem->removeGlobalHandler( m_globalKeyHandlerF9 );

        //    if( m_debugText != nullptr )
        //    {
        //        m_debugText->disable();
        //        m_debugText = nullptr;
        //    }

        //    const SchedulerInterfacePtr & scheduler = PLAYER_SERVICE()
        //        ->getGlobalScheduler();

        //    scheduler->remove( m_schedulerFPS );
        //}
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugRenderService::renderDebugNode( const NodePtr & _node, const RenderContext * _context, bool _external )
    {
        if( _node->isActivate() == false )
        {
            return;
        }

        RenderInterface * selfRender = _node->getRender();

        if( selfRender != nullptr )
        {
            if( selfRender->isRenderEnable() == false )
            {
                return;
            }

            if( selfRender->isHide() == true )
            {
                return;
            }

            if( selfRender->isLocalTransparent() == true )
            {
                return;
            }

            if( selfRender->isExternalRender() == true && _external == false )
            {
                return;
            }

            RenderContext self_context;

            const RenderViewportInterfacePtr & renderViewport = selfRender->getRenderViewport();

            if( renderViewport != nullptr )
            {
                self_context.viewport = renderViewport;
            }
            else
            {
                self_context.viewport = _context->viewport;
            }

            const RenderCameraInterfacePtr & renderCamera = selfRender->getRenderCamera();

            if( renderCamera != nullptr )
            {
                self_context.camera = renderCamera;
            }
            else
            {
                self_context.camera = _context->camera;
            }

            const RenderScissorInterfacePtr & renderScissor = selfRender->getRenderScissor();

            if( renderScissor != nullptr )
            {
                self_context.scissor = renderScissor;
            }
            else
            {
                self_context.scissor = _context->scissor;
            }

            const RenderTargetInterfacePtr & renderTarget = selfRender->getRenderTarget();

            if( renderTarget != nullptr )
            {
                self_context.target = renderTarget;
            }
            else
            {
                self_context.target = _context->target;
            }

            if( selfRender->isLocalHide() == false && selfRender->isPersonalTransparent() == false )
            {
                selfRender->render( &self_context );

                const ConstString & type = _node->getType();

                const NodeDebugRenderInterfacePtr & nodeDebugRender = m_nodeDebugRenders.find( type );

                if( nodeDebugRender != nullptr )
                {
                    nodeDebugRender->render( &self_context, _node.get() );
                }
            }

            const RenderContext * children_context = &self_context;
            _node->foreachChildrenUnslug( [this, children_context]( const NodePtr & _child )
            {
                this->renderDebugNode( _child, children_context, false );
            } );

            if( self_context.target != nullptr )
            {
                const RenderInterfacePtr & targetRender = selfRender->makeTargetRender( &self_context );

                if( targetRender != nullptr )
                {
                    targetRender->render( _context );
                }
            }
        }
        else
        {
            _node->foreachChildrenUnslug( [this, _context]( const NodePtr & _child )
            {
                this->renderDebugNode( _child, _context, false );
            } );

            const ConstString & type = _node->getType();

            const NodeDebugRenderInterfacePtr & nodeDebugRender = m_nodeDebugRenders.find( type );

            if( nodeDebugRender != nullptr )
            {
                nodeDebugRender->render( _context, _node.get() );
            }
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
    void NodeDebugRenderService::renderDebugInfo( const RenderContext * _context )
    {
        MENGINE_UNUSED( _context );

//        bool developmentMode = HAS_OPTION( "dev" );
//
//        if( developmentMode == false )
//        {
//            return;
//        }
//
//        if( m_showDebugText == 0 )
//        {
//            RENDER_SERVICE()
//                ->enableDebugFillrateCalcMode( false );
//        }
//
//        if( m_showDebugText != 0 )
//        {
//            const RenderServiceDebugInfo & rdi = RENDER_SERVICE()
//                ->getDebugInfo();
//
//            Stringstream ss;
//
//            ss << "FPS: " << m_fps << std::endl;
//
//            if( m_showDebugText > 1 )
//            {
//                RENDER_SERVICE()
//                    ->enableDebugFillrateCalcMode( true );
//
//                const Resolution & contentResolution = APPLICATION_SERVICE()
//                    ->getContentResolution();
//
//                double sreenfillrate = rdi.fillrate / double( contentResolution.getWidth() * contentResolution.getHeight() );
//
//                ss << "Fillrate " << std::setiosflags( std::ios::fixed ) << std::setprecision( 2 ) << sreenfillrate << " (Object " << rdi.object << " Triangle " << rdi.triangle << ")" << std::endl;
//                ss << "DIP: " << rdi.dips << std::endl;
//
//                ERenderBatchMode mode = RENDER_SERVICE()
//                    ->getBatchMode();
//
//                ss << "Smart Batch: " << mode << " " << rdi.batch << std::endl;
//
//                uint32_t textureMemoryUse = RENDER_SYSTEM()
//                    ->getTextureMemoryUse();
//
//                ss << "Texture Memory Usage: " << (float)textureMemoryUse / (1024.f*1024.f) << std::endl;
//
//                uint32_t textureCount = RENDER_SYSTEM()
//                    ->getTextureCount();
//
//                ss << "Texture Count: " << textureCount << std::endl;
//
//                if( SERVICE_EXIST( Mengine::AstralaxSystemInterface ) == true )
//                {
//                    uint32_t particlesCount = ASTRALAX_SYSTEM()
//                        ->getEmitterCount();
//
//                    ss << "Particles: " << particlesCount << std::endl;
//                }
//            }
//
//            if( m_showDebugText == 2 )
//            {
//                class CompileResourceVisitor
//                    : public Visitor
//                    , public Factorable
//                    , public ConcreteVisitor<Resource>
//                {
//                public:
//                    CompileResourceVisitor()
//                        : m_count( 0 )
//                    {
//                    }
//
//                public:
//                    uint32_t getCount() const
//                    {
//                        return m_count;
//                    }
//
//                protected:
//                    void accept( Resource * _resource )
//                    {
//                        if( _resource->isCompile() == false )
//                        {
//                            return;
//                        }
//
//                        ++m_count;
//                    }
//
//                protected:
//                    uint32_t m_count;
//                };
//
//                typedef IntrusivePtr<CompileResourceVisitor> CompileResourceVisitorPtr;
//
//                CompileResourceVisitorPtr crv = Helper::makeFactorableUnique<CompileResourceVisitor>();
//
//                RESOURCE_SERVICE()
//                    ->visitResources( crv );
//
//                ss << "Resources: " << crv->getCount() << std::endl;
//
//                class CompleteThreadTaskVisitor
//                    : public Visitor
//                    , public Factorable
//                    , public ConcreteVisitor<ThreadTask>
//                {
//                public:
//                    CompleteThreadTaskVisitor()
//                        : m_count( 0 )
//                    {
//                    }
//
//                public:
//                    uint32_t getCount() const
//                    {
//                        return m_count;
//                    }
//
//                protected:
//                    void accept( ThreadTask * _task )
//                    {
//                        if( _task->isComplete() == false )
//                        {
//                            return;
//                        }
//
//                        ++m_count;
//                    }
//
//                protected:
//                    uint32_t m_count;
//                };
//
//                typedef IntrusivePtr<CompleteThreadTaskVisitor> CompleteThreadTaskVisitorPtr;
//
//                CompleteThreadTaskVisitorPtr cttv = Helper::makeFactorableUnique<CompleteThreadTaskVisitor>();
//
//                PREFETCHER_SERVICE()
//                    ->visitPrefetches( cttv );
//
//                ss << "Prefetcher " << cttv->getCount() << std::endl;
//
//                ss << "PickerTrapCount:" << PICKER_SERVICE()->getPickerTrapCount() << std::endl;
//            }
//            else if( m_showDebugText == 3 )
//            {
//                //////////////////////////////////////////////////////////////////////////
//                class VisitorPlayerFactoryManager
//                {
//                public:
//                    VisitorPlayerFactoryManager( const ConstString & _category )
//                        : m_category( _category )
//                    {
//                    }
//
//
//                public:
//                    void visit( const PrototypeGeneratorInterfacePtr & _generator )
//                    {
//                        const ConstString & category = _generator->getCategory();
//                        const ConstString & prototype = _generator->getPrototype();
//
//                        if( m_category != category )
//                        {
//                            return;
//                        }
//
//                        uint32_t count = _generator->count();
//
//                        if( count == 0 )
//                        {
//                            return;
//                        }
//
//                        m_scopes[count].emplace_back( prototype );
//                    }
//
//                public:
//                    String getMsg() const
//                    {
//                        Stringstream ss;
//
//                        uint32_t iterator = 0;
//
//                        for( MapPybindScope::const_reverse_iterator
//                            it = m_scopes.rbegin(),
//                            it_end = m_scopes.rend();
//                            it != it_end;
//                            ++it )
//                        {
//                            uint32_t c = it->first;
//                            const VectorConstString & l = it->second;
//
//                            for( const ConstString & s : l )
//                            {
//                                ss << "Prototype: " << s.c_str() << " " << c << std::endl;
//
//                                if( ++iterator == 15 )
//                                {
//                                    break;
//                                }
//                            }
//
//                            if( iterator == 15 )
//                            {
//                                break;
//                            }
//
//                        }
//
//                        return ss.str();
//                    }
//
//                protected:
//                    ConstString m_category;
//                    typedef Map<uint32_t, VectorConstString> MapPybindScope;
//                    MapPybindScope m_scopes;
//                };
//
//                VisitorPlayerFactoryManager pfmv_node( STRINGIZE_STRING_LOCAL( "Node" ) );
//
//                PROTOTYPE_SERVICE()
//                    ->foreachGenerators( [&pfmv_node]( const PrototypeGeneratorInterfacePtr & _generator ) { pfmv_node.visit( _generator ); } );
//
//                ss << pfmv_node.getMsg() << std::endl;
//
//                VisitorPlayerFactoryManager pfmv_surface( STRINGIZE_STRING_LOCAL( "Surface" ) );
//
//                PROTOTYPE_SERVICE()
//                    ->foreachGenerators( [&pfmv_surface]( const PrototypeGeneratorInterfacePtr & _generator ) { pfmv_surface.visit( _generator ); } );
//
//                ss << pfmv_surface.getMsg() << std::endl;
//            }
//            else if( m_showDebugText == 4 )
//            {
//                class MyVisitorPythonClassTypeScope
//                    : public pybind::visitor_class_type_scope
//                {
//                public:
//                    MyVisitorPythonClassTypeScope()
//                    {
//                    }
//
//                protected:
//                    void operator = ( const MyVisitorPythonClassTypeScope & )
//                    {
//                    }
//
//                protected:
//                    void visit_scope( pybind::class_type_scope_interface * _scope ) override
//                    {
//#ifdef MENGINE_DEBUG
//                        uint32_t count = _scope->getObjectCount();
//#else
//                        uint32_t count = 0;
//#endif
//
//                        if( count == 0 )
//                        {
//                            return;
//                        }
//
//                        String name = _scope->get_name();
//
//                        m_scopes[count].emplace_back( name );
//                    }
//
//                public:
//                    String getMsg() const
//                    {
//                        Stringstream ss;
//
//                        uint32_t iterator = 0;
//
//                        for( MapPybindScope::const_reverse_iterator
//                            it = m_scopes.rbegin(),
//                            it_end = m_scopes.rend();
//                            it != it_end;
//                            ++it )
//                        {
//                            uint32_t c = it->first;
//                            const VectorString & l = it->second;
//
//                            for( const String & s : l )
//                            {
//                                ss << "Python: " << s << " " << c << std::endl;
//
//                                if( ++iterator == 15 )
//                                {
//                                    break;
//                                }
//                            }
//
//                            if( iterator == 15 )
//                            {
//                                break;
//                            }
//                        }
//
//                        return ss.str();
//                    }
//
//                protected:
//                    typedef Map<uint32_t, VectorString> MapPybindScope;
//                    MapPybindScope m_scopes;
//                };
//
//                pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()
//                    ->getKernel();
//
//                MyVisitorPythonClassTypeScope mvcts;
//                kernel->visit_types_scope( &mvcts );
//
//                String msg_python = mvcts.getMsg();
//                ss << msg_python << std::endl;
//            }
//            else if( m_showDebugText == 5 )
//            {
//                class MyVisitorFactoryService
//                    : public VisitorFactoryInterface
//                    , public Factorable
//                {
//                public:
//                    MyVisitorFactoryService()
//                    {
//                    }
//
//                protected:
//                    void operator = ( const MyVisitorFactoryService & )
//                    {
//                    }
//
//                protected:
//                    void visit( const Factory * _factory )
//                    {
//                        uint32_t count = _factory->getCountObject();
//
//                        if( count == 0 )
//                        {
//                            return;
//                        }
//
//                        String name = _factory->getName();
//
//                        m_scopes[count].emplace_back( name );
//                    }
//
//                public:
//                    String getMsg() const
//                    {
//                        Stringstream ss;
//
//                        uint32_t iterator = 0;
//
//                        for( MapPybindScope::const_reverse_iterator
//                            it = m_scopes.rbegin(),
//                            it_end = m_scopes.rend();
//                            it != it_end;
//                            ++it )
//                        {
//                            uint32_t c = it->first;
//                            const VectorString & l = it->second;
//
//                            for( const String & s : l )
//                            {
//                                ss << "Factory: " << s << " " << c << std::endl;
//
//                                if( ++iterator == 15 )
//                                {
//                                    break;
//                                }
//                            }
//
//                            if( iterator == 15 )
//                            {
//                                break;
//                            }
//                        }
//
//                        return ss.str();
//                    }
//
//                protected:
//                    typedef Map<uint32_t, VectorString> MapPybindScope;
//                    MapPybindScope m_scopes;
//                };
//
//                typedef IntrusivePtr<MyVisitorFactoryService> MyVisitorFactoryServicePtr;
//
//                MyVisitorFactoryServicePtr mvcts = Helper::makeFactorableUnique<MyVisitorFactoryService>();
//                FACTORY_SERVICE()
//                    ->visitFactories( mvcts );
//
//                String msg_python = mvcts->getMsg();
//                ss << msg_python << std::endl;
//            }
//            else if( m_showDebugText == 6 )
//            {
//                typedef Map<int32_t, VectorString> MapPybindScope;
//                MapPybindScope scopes;
//
//                uint32_t count = stdex_allocator_report_count();
//
//                for( uint32_t i = 0; i != count; ++i )
//                {
//                    stdex_memory_report_t * report = stdex_allocator_report_info( i );
//
//                    scopes[report->count].emplace_back( report->name );
//                }
//
//                Stringstream ss2;
//
//                uint32_t iterator = 0;
//
//                for( MapPybindScope::const_reverse_iterator
//                    it = scopes.rbegin(),
//                    it_end = scopes.rend();
//                    it != it_end;
//                    ++it )
//                {
//                    int32_t c = it->first;
//                    const VectorString & l = it->second;
//
//                    for( const String & s : l )
//                    {
//                        ss2 << "Memory: " << s << " " << c << std::endl;
//
//                        if( iterator++ == 15 )
//                        {
//                            break;
//                        }
//                    }
//
//                    if( iterator == 15 )
//                    {
//                        break;
//                    }
//                }
//
//                String msg_python = ss2.str();
//                ss << msg_python << std::endl;
//            }
//
//            String text = ss.str();
//
//            VectorString args;
//            args.emplace_back( text );
//            m_debugText->setTextFormatArgs( args );
//
//            float gameViewportAspect;
//            Viewport gameViewport;
//
//            APPLICATION_SERVICE()
//                ->getGameViewport( gameViewportAspect, gameViewport );
//
//            m_debugText->setLocalPosition( mt::vec3f( gameViewport.begin, 0.f ) );
//
//            const Resolution & resolution = APPLICATION_SERVICE()
//                ->getCurrentResolution();
//
//            const Resolution & content = APPLICATION_SERVICE()
//                ->getContentResolution();
//
//            mt::vec2f scale;
//            content.calcScale( resolution, scale );
//
//            m_debugText->setLocalScale( mt::vec3f( scale, 1.f ) );
//
//            m_debugText->render( _context );
//        }
    }
    //////////////////////////////////////////////////////////////////////////     
    void NodeDebugRenderService::toggleDebugText_()
    {
        ++m_showDebugText;
        m_showDebugText %= 7;
    }
}