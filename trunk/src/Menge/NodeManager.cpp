#	include "NodeManager.h"

#   include "Interface/StringizeInterface.h"

#   include "Logger/Logger.h"

#	include "Kernel/Node.h"

#	include <memory>

SERVICE_FACTORY(NodeService, Menge::NodeServiceInterface, Menge::NodeManager);

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	NodeManager::NodeManager()
        : m_serviceProvider(NULL)
        , m_homeless(0)
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void NodeManager::initialize()
    {
        m_homeless = new Node();

        m_homeless->setName( Helper::stringizeString(m_serviceProvider, "Homeless") );
        m_homeless->setType( Helper::stringizeString(m_serviceProvider, "Node") );
        m_homeless->setServiceProvider( m_serviceProvider );        
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeManager::finalize()
    {
        if( m_homeless )
        {
            m_homeless->destroyAllChild();
            delete m_homeless;
            m_homeless = NULL;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeManager::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;

        FactoryManager::setServiceProvider( m_serviceProvider );
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * NodeManager::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	Node * NodeManager::createNode( const ConstString& _type )
	{
		Node * node = FactoryManager::createObjectT<Node>( _type );

		if( node == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("NodeManager::createNode: Invalid Node Type '%s'"
				, _type.c_str() 
				);

			return 0;
		}

		node->setType( _type );

        node->setServiceProvider( m_serviceProvider );
		
		return node;
	}
    //////////////////////////////////////////////////////////////////////////
    void NodeManager::addHomeless( Node * _homeless )
    {
        if( m_homeless == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)( "NodeManager::addHomeless: not initialize"
                );

            return;
        }

        _homeless->release();

        if( _homeless->hasParent() == true )
        {
            Node * parent = _homeless->getParent();

            LOGGER_ERROR(m_serviceProvider)( "NodeManager::addHomeless: '%s' have parent '%s'"
                , _homeless->getName().c_str()
                , parent->getName().c_str()
                );

            return;
        }

        m_homeless->addChildren( _homeless );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeManager::clearHomeless()
    {
        m_homeless->destroyAllChild();
    }
}
