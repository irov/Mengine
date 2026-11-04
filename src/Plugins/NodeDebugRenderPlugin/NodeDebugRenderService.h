#pragma once

#include "Interface/PlayerSceneRenderProviderInterface.h"

#include "NodeDebugRenderServiceInterface.h"

#include "Engine/TextField.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Hashtable.h"
#include "Kernel/GlobalHotkeys.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class NodeDebugRenderService
        : public ServiceBase<NodeDebugRenderServiceInterface>
        , public PlayerSceneRenderProviderInterface
    {
        DECLARE_FACTORABLE( NodeDebugRenderService );

    public:
        NodeDebugRenderService();
        ~NodeDebugRenderService() override;

    public:
        const ServiceRequiredList & requiredServices() const override;
        bool _availableService() const override;
        void _dependencyService() override;
        bool _initializeService() override;
        void _finalizeService() override;
        void _stopService() override;

    public:
        void renderDebugNode( const NodePtr & _node, const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, bool _external, bool _hide ) override;

    public:
        void addNodeDebugRender( const ConstString & _type, const NodeDebugRenderInterfacePtr & _nodeDebugRender ) override;
        void removeNodeDebugRender( const ConstString & _type ) override;

    public:
        void updateDebugInfo( const UpdateContext * _context ) override;
        void renderDebugInfo( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) override;

    protected:
        void renderScene( const NodePtr & _scene, const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) override;
        void renderArrow( const NodePtr & _arrow, const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) override;

    protected:
        void toggleDebugText_();
        void toggleDebugMask_( uint32_t _mask );
        void toggleWireframe_();
        void updateSceneRenderProvider_();

    protected:
        typedef Hashtable<ConstString, NodeDebugRenderInterfacePtr> HashtableNodeDebugRenders;
        HashtableNodeDebugRenders m_nodeDebugRenders;

        GlobalHotkeys m_hotkeys;

        uint32_t m_fps;

        uint32_t m_showDebugText;
        uint32_t m_debugMask;
        bool m_wireframe;

        UniqueId m_timerFPS;
    };
}