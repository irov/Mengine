#pragma once

#include "ScriptablePrototypeGenerator.h"

#include "Interface/EnumeratorServiceInterface.h"

#include "Kernel/Node.h"
#include "Kernel/ConstString.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    template<class Type, uint32_t Count>
    class NodePrototypeGenerator
        : public ScriptablePrototypeGenerator<Type, Count>
    {
    protected:
        FactorablePointer generate( const Char * _doc ) override
        {
            const FactoryPtr & factory = this->getFactory();

            NodePtr node = factory->createObject( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( node, nullptr, "can't generate '%s' '%s' doc '%s'"
                , this->getCategory().c_str()
                , this->getPrototype().c_str()
                , _doc
            );

            const ConstString & prototype = this->getPrototype();
            node->setType( prototype );

            uint32_t uniqueIdentity = GENERATE_UNIQUE_IDENTITY();
            node->setUniqueIdentity( uniqueIdentity );

            this->setupScriptable( node );

            return node;
        }
    };
}
