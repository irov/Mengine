#   pragma once

#   include "Interface/ServiceInterface.h"

#   include "Core/ConstString.h"

namespace Menge
{
    class Node;
    class Factory;

    class NodeServiceInterface
        : public ServiceInterface
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

#   ifdef _DEBUG
            if( dynamic_cast<T *>(node) == nullptr )
            {
                return nullptr;
            }
#   endif

            T * t = static_cast<T*>(node);

            return t;
        }

    public:
        virtual void addHomeless( Node * _homeless ) = 0;
        virtual void clearHomeless() = 0;
    };

#   define NODE_SERVICE( serviceProvider )\
    (Menge::Helper::getService<Menge::NodeServiceInterface>(serviceProvider))
}