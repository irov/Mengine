#pragma once

#include "ScriptablePrototypeGenerator.h"

#include "Interface/EnumeratorServiceInterface.h"

#include "Kernel/Node.h"

#include "Kernel/ConstString.h"
#include "Kernel/MemoryAllocator.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    template<class Type, uint32_t Count>
    class NodePrototypeGenerator
        : public ScriptablePrototypeGenerator<Type, Count>
    {
    protected:
        FactorablePointer generate() override
        {
            const FactoryPtr & factory = this->getFactory();

            NodePtr node = factory->createObject();

            if( node == nullptr )
            {
                LOGGER_ERROR( "NodePrototypeGenerator::generate can't generate %s %s"
                    , this->getCategory().c_str()
                    , this->getPrototype().c_str()
                );

                return nullptr;
            }

            const ConstString & prototype = this->getPrototype();
            node->setType( prototype );

            uint32_t uniqueIdentity = ENUMERATOR_SERVICE()
                ->generateUniqueIdentity();

            node->setUniqueIdentity( uniqueIdentity );

            this->setupScriptable( node );

            return node;
        }
    };
}
