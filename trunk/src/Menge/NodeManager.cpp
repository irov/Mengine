#	include "NodeManager.h"

#   include "Interface/StringizeInterface.h"
#   include "Interface/PrototypeManagerInterface.h"

#   include "Logger/Logger.h"

#	include "Kernel/Node.h"
#	include "Consts.h"

#	include <memory>

SERVICE_FACTORY(NodeService, Menge::NodeServiceInterface, Menge::NodeManager);

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	NodeManager::NodeManager()
        : m_serviceProvider(nullptr)
        , m_homeless(nullptr)
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void NodeManager::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * NodeManager::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeManager::initialize()
    {
        m_homeless = new Node();

        m_homeless->setName( STRINGIZE_STRING_LOCAL(m_serviceProvider, "Homeless") );
        m_homeless->setType( STRINGIZE_STRING_LOCAL(m_serviceProvider, "Node") );
        m_homeless->setServiceProvider( m_serviceProvider );        
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeManager::finalize()
    {
        if( m_homeless != nullptr )
        {
            Node * homeless = m_homeless;
            m_homeless = nullptr;
			
            homeless->destroyAllChild();
            delete homeless;            
        }
    }
	//////////////////////////////////////////////////////////////////////////
	Node * NodeManager::createNode( const ConstString& _type )
	{
		Node * node = PROTOTYPE_SERVICE(m_serviceProvider)
            ->generatePrototypeT<Node>( CONST_STRING(m_serviceProvider, Node), _type );

		if( node == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("NodeManager::createNode: Invalid Node Type '%s'"
				, _type.c_str() 
				);

			return nullptr;
		}

		node->setType( _type );

        node->setServiceProvider( m_serviceProvider );
		
		return node;
	}
    //////////////////////////////////////////////////////////////////////////
    void NodeManager::addHomeless( Node * _homeless )
    {
        if( m_homeless == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("NodeManager::addHomeless: not initialize"
                );

            return;
        }

        //_homeless->release();

		if( _homeless->hasParent() == true )
		{
			Node * parent = _homeless->getParent();

			LOGGER_WARNING(m_serviceProvider)("NodeManager::addHomeless: '%s:%s' have parent '%s:%s'"
				, _homeless->getName().c_str()
				, _homeless->getType().c_str()
				, parent->getName().c_str()
				, parent->getType().c_str()
				);
		}

        m_homeless->addChild( _homeless );
    }
	//////////////////////////////////////////////////////////////////////////
	bool NodeManager::isHomeless( Node * _node ) const
	{
		if( _node == nullptr )
		{
			return false;
		}

		Node * parent = _node->getParent();

		bool is = parent == m_homeless;

		return is;
	}
    //////////////////////////////////////////////////////////////////////////
    void NodeManager::clearHomeless()
    {
        m_homeless->destroyAllChild();
    }
}
