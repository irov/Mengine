#include "NodeManager.h"

#include "Interface/StringizeInterface.h"
#include "Interface/PrototypeManagerInterface.h"

#include "Kernel/FactorableUnique.h"

#include "Kernel/Logger.h"

#include "Kernel/Node.h"

#include <memory>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( NodeService, Mengine::NodeManager);
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	NodeManager::NodeManager()
        : m_homeless(nullptr)
	{
	}
    //////////////////////////////////////////////////////////////////////////
    bool NodeManager::_initializeService()
    {
        m_homeless = new FactorableUnique<Node>();

		if( m_homeless == nullptr )
		{
			return false;
		}

        m_homeless->setName( STRINGIZE_STRING_LOCAL( "Homeless") );
        m_homeless->setType( STRINGIZE_STRING_LOCAL( "Node") );
        
		return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeManager::_finalizeService()
    {
        if( m_homeless != nullptr )
        {
            NodePtr homeless = m_homeless;
            m_homeless = nullptr;
			
            homeless->destroyAllChild();
        }
    }
	//////////////////////////////////////////////////////////////////////////
    PointerNode NodeManager::createNode( const ConstString& _type )
	{
        NodePtr node = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), _type );

		if( node == nullptr )
		{
			LOGGER_ERROR("NodeManager::createNode: Invalid Node Type '%s'"
				, _type.c_str() 
				);

			return nullptr;
		}
				
		return node;
	}
    //////////////////////////////////////////////////////////////////////////
    void NodeManager::addHomeless( const NodePtr & _homeless )
    {
        if( m_homeless == nullptr )
        {
            LOGGER_ERROR("NodeManager::addHomeless: not initialize"
                );

            return;
        }

        //_homeless->release();

		if( _homeless->hasParent() == true )
		{
			Node * parent = _homeless->getParent();

			LOGGER_WARNING("NodeManager::addHomeless: '%s:%s' have parent '%s:%s'"
				, _homeless->getName().c_str()
				, _homeless->getType().c_str()
				, parent->getName().c_str()
				, parent->getType().c_str()
				);
		}

        m_homeless->addChild( _homeless );
    }
	//////////////////////////////////////////////////////////////////////////
	bool NodeManager::isHomeless( const NodePtr & _node ) const
	{
		if( _node == nullptr )
		{
			return false;
		}

		Node * parent = _node->getParent();

		bool is = (parent == m_homeless);

		return is;
	}
    //////////////////////////////////////////////////////////////////////////
    void NodeManager::clearHomeless()
    {
        m_homeless->destroyAllChild();
    }
}
