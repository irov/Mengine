#pragma once

#include "Interface/NodeInterface.h"

#include "Config/Typedef.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/ConstString.h"
#include "Kernel/IntrusivePtr.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class NodeService
        : public ServiceBase<NodeServiceInterface>
    {
    public:
        NodeService();
        ~NodeService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        PointerNode createNode( const ConstString& _type ) override;

    public:
        void addHomeless( const NodePtr & _homeless ) override;
        bool isHomeless( const NodePtr & _node ) const override;
        void clearHomeless() override;

    protected:
        NodePtr m_homeless;
    };
}
