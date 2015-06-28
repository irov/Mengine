#	pragma once

#   include "Interface/NodeInterface.h"

#	include "Config/Typedef.h"

#	include "Core/ConstString.h"


namespace Menge
{
	class Node;	
	class NodeFactory;

	class NodeManager
        : public NodeServiceInterface
	{
	public:
		NodeManager();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    public:
        void initialize() override;
        void finalize() override;

    public:
		Node * createNode( const ConstString& _type ) override;

    public:
        void addHomeless( Node * _homeless ) override;
		bool isHomeless( Node * _node ) const override;
        void clearHomeless() override;

    protected:
        ServiceProviderInterface * m_serviceProvider;

        Node * m_homeless;
	};
}
