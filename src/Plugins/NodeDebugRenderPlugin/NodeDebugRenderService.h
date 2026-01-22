#pragma once

#include "NodeDebugRenderServiceInterface.h"

#include "Engine/TextField.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Hashtable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class NodeDebugRenderService
        : public ServiceBase<NodeDebugRenderServiceInterface>
    {
        DECLARE_FACTORABLE( NodeDebugRenderService );

    public:
        NodeDebugRenderService();
        ~NodeDebugRenderService() override;

    public:
        bool _availableService() const override;
        void _dependencyService() override;
        bool _initializeService() override;
        void _finalizeService() override;
        void _stopService() override;

    public:
        void renderDebugArrow( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, bool _external, bool _hide ) override;
        void renderDebugNode( const NodePtr & _node, const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, bool _external, bool _hide ) override;

    public:
        void addNodeDebugRender( const ConstString & _type, const NodeDebugRenderInterfacePtr & _nodeDebugRender ) override;
        void removeNodeDebugRender( const ConstString & _type ) override;

    public:
        void updateDebugInfo( const UpdateContext * _context ) override;
        void renderDebugInfo( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) override;

    protected:
        void toggleDebugText_();

    protected:
        typedef Hashtable<ConstString, NodeDebugRenderInterfacePtr> HashtableNodeDebugRenders;
        HashtableNodeDebugRenders m_nodeDebugRenders;

        uint32_t m_fps;

        uint32_t m_showDebugText;
        UniqueId m_globalKeyHandlerF9;

        UniqueId m_timerFPS;
    };
}