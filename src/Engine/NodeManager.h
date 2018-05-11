#pragma once

#include "Interface/NodeInterface.h"

#include "Config/Typedef.h"

#include "Core/ServiceBase.h"
#include "Core/ConstString.h"
#include "Core/IntrusivePtr.h"

namespace Mengine
{	
    //////////////////////////////////////////////////////////////////////////
	class NodeManager
        : public ServiceBase<NodeServiceInterface>
	{
	public:
		NodeManager();

    public:
        bool _initialize() override;
        void _finalize() override;

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
