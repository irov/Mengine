#	pragma once

#   include "Interface/NodeInterface.h"

#	include "Config/Typedef.h"

#	include "Core/ConstString.h"

#	include "Factory/FactoryManager.h"


namespace Menge
{
	class Node;	
	class NodeFactory;

	class NodeManager
        : public NodeServiceInterface
		, public FactoryManager
	{
	public:
		NodeManager();

    public:
        void initialize() override;
        void finalize() override;

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		Node * createNode( const ConstString& _type ) override;

    public:
        void addHomeless( Node * _homeless ) override;
        void clearHomeless() override;

    protected:
        ServiceProviderInterface * m_serviceProvider;

        Node * m_homeless;
	};
}
