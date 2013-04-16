#   pragma once

#   include "Interface/ServiceInterface.h"
#   include "Interface/FactoryManagerInterface.h"

#   include "Core/ConstString.h"

namespace Menge
{
    class Node;
    class Factory;
    
    typedef std::vector<Node*> TVectorNode;

    class NodeServiceInterface
        : public ServiceInterface
        , virtual public FactoryManagerInterface
    {
        SERVICE_DECLARE("NodeService")
                
    public:
        virtual void initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual Node * createNode( const ConstString& _type ) = 0;

        template<class T>
        T * createNodeT( const ConstString& _type )
        {
            Node * node = this->createNode( _type );

            return static_cast<T*>(node);
        }

    public:
        virtual void addHomeless( Node * _homeless ) = 0;
        virtual void clearHomeless() = 0;
    };

#   define NODE_SERVICE( serviceProvider )\
    (Menge::getService<Menge::NodeServiceInterface>(serviceProvider))
}