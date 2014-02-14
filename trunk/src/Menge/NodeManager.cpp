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

        m_homeless->setName( Helper::stringizeString(m_serviceProvider, "Homeless") );
        m_homeless->setType( Helper::stringizeString(m_serviceProvider, "Node") );
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

        _homeless->release();

		//if( _homeless->hasParent() == true )
		//{
		//	Node * parent = _homeless->getParent();

		//	LOGGER_WARNING(m_serviceProvider)("NodeManager::addHomeless: '%s' have parent '%s'"
		//		, _homeless->getName().c_str()
		//		, parent->getName().c_str()
		//		);
		//}

        m_homeless->addChildren( _homeless );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeManager::clearHomeless()
    {
        m_homeless->destroyAllChild();
    }
}
