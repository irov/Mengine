#   pragma once

#include "Interface/ServiceInterface.h"

#include "Core/ConstString.h"

namespace Mengine
{
    class Node;
    class Factory;

    class NodeServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("NodeService")

    public:
        virtual Node * createNode( const ConstString& _type ) = 0;

        template<class T>
        T createNodeT( const ConstString& _type )
        {
            Node * node = this->createNode( _type );

#   ifdef _DEBUG
            if( dynamic_cast<T>(node) == nullptr )
            {
                throw;
            }
#   endif

            T t = static_cast<T>(node);

            return t;
        }

    public:
        virtual void addHomeless( Node * _homeless ) = 0;
		virtual bool isHomeless( const Node * _node ) const = 0;
        virtual void clearHomeless() = 0;
    };

#   define NODE_SERVICE()\
    ((Mengine::NodeServiceInterface *)SERVICE_GET(Mengine::NodeServiceInterface))
}