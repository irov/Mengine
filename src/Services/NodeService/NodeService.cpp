#include "NodeService.h"

#include "Interface/StringizeInterface.h"
#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/FactorableUnique.h"

#include "Kernel/Logger.h"

#include "Kernel/Node.h"

#include <memory>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( NodeService, Mengine::NodeService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    NodeService::NodeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    NodeService::~NodeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeService::_initializeService()
    {
        m_shelter = new FactorableUnique<Node>();

        if( m_shelter == nullptr )
        {
            return false;
        }

        m_shelter->setName( STRINGIZE_STRING_LOCAL( "Homeless" ) );
        m_shelter->setType( STRINGIZE_STRING_LOCAL( "Node" ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeService::_finalizeService()
    {
        if( m_shelter != nullptr )
        {
            NodePtr homeless = m_shelter;
            m_shelter = nullptr;

            homeless->destroyChildren( []( const NodePtr & ) {} );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    PointerNode NodeService::createNode( const ConstString& _type )
    {
        NodePtr node = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), _type );

        if( node == nullptr )
        {
            LOGGER_ERROR( "NodeManager::createNode: Invalid Node Type '%s'"
                , _type.c_str()
            );

            return nullptr;
        }

        return node;
    }
    //////////////////////////////////////////////////////////////////////////
    const NodePtr & NodeService::getShelter() const
    {
        return m_shelter;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeService::addHomeless( const NodePtr & _homeless )
    {
        if( m_shelter == nullptr )
        {
            LOGGER_ERROR( "NodeManager::addHomeless: not initialize"
            );

            return;
        }

        //_homeless->release();

        if( _homeless->hasParent() == true )
        {
            Node * parent = _homeless->getParent();

            LOGGER_WARNING( "NodeManager::addHomeless: '%s:%s' have parent '%s:%s'"
                , _homeless->getName().c_str()
                , _homeless->getType().c_str()
                , parent->getName().c_str()
                , parent->getType().c_str()
            );
        }

        m_shelter->addChild( _homeless );
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeService::isHomeless( const NodePtr & _node ) const
    {
        if( _node == nullptr )
        {
            return false;
        }

        Node * parent = _node->getParent();

        if( parent != m_shelter )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeService::clearHomeless()
    {
        m_shelter->destroyChildren( []( const NodePtr & ) {} );
    }
}
