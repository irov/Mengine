#	pragma once

#   include "Interface/NodeInterface.h"

#	include "Config/Typedef.h"

#	include "Core/ConstString.h"


namespace Menge
{
	class Node;	
	class NodeFactory;

	class NodeManager
        : public ServiceBase<NodeServiceInterface>
	{
	public:
		NodeManager();

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
		Node * createNode( const ConstString& _type ) override;

    public:
        void addHomeless( Node * _homeless ) override;
		bool isHomeless( Node * _node ) const override;
        void clearHomeless() override;

    protected:
        Node * m_homeless;
	};
}
