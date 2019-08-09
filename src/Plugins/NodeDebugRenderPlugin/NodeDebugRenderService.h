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
    public:
        NodeDebugRenderService();
        ~NodeDebugRenderService() override;

    public:
        bool _availableService() const override;
        void _dependencyService() override;
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void renderDebugNode( const NodePtr & _node, const RenderContext * _context, bool _external ) override;

    public:
        void addNodeDebugRender( const ConstString & _type, const NodeDebugRenderInterfacePtr & _nodeDebugRender ) override;
        void removeNodeDebugRender( const ConstString & _type ) override;

    public:
        void updateDebugInfo( const UpdateContext * _context ) override;
        void renderDebugInfo( const RenderContext * _context ) override;

    protected:
        void toggleDebugText_();

    protected:
        typedef Hashtable<ConstString, NodeDebugRenderInterfacePtr> HashtableNodeDebugRenders;
        HashtableNodeDebugRenders m_nodeDebugRenders;

        TextFieldPtr m_debugText;

        uint32_t m_fps;

        uint32_t m_showDebugText;
        uint32_t m_globalKeyHandlerF9;

        uint32_t m_schedulerFPS;
    };
}