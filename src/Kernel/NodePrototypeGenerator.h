#pragma once

#ifdef MENGINE_USE_SCRIPT_SERVICE
#include "Kernel/ScriptablePrototypeGenerator.h"
#else
#include "Kernel/DefaultPrototypeGenerator.h"
#endif

#include "Interface/EnumeratorServiceInterface.h"

#include "Kernel/Node.h"
#include "Kernel/ConstString.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    template<class Type, uint32_t Count>
    class NodePrototypeGenerator
#ifdef MENGINE_USE_SCRIPT_SERVICE
        : public ScriptablePrototypeGenerator<Type, Count>
#else
        : public DefaultPrototypeGenerator<Type, Count>
#endif
    {
    public:
        NodePrototypeGenerator()
        {
        }

        ~NodePrototypeGenerator() override
        {
        }

    protected:
        FactorablePointer generate( const DocumentPtr & _doc ) override
        {
            const FactoryPtr & factory = this->getFactory();

            NodePtr node = factory->createObject( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( node, "can't generate '%s' '%s' doc '%s'"
                , this->getCategory().c_str()
                , this->getPrototype().c_str()
                , MENGINE_DOCUMENT_STR( _doc )
            );

            const ConstString & prototype = this->getPrototype();
            node->setType( prototype );

            uint32_t uniqueIdentity = GENERATE_UNIQUE_IDENTITY();
            node->setUniqueIdentity( uniqueIdentity );

#ifdef MENGINE_DEBUG
            DocumentPtr doc = MENGINE_DOCUMENT_MESSAGE( "Node '%s' type '%s'"
                , node->getName().c_str()
                , node->getType().c_str()
            );

            doc->setParent( _doc );

            node->setDocument( doc );
#endif

#ifdef MENGINE_USE_SCRIPT_SERVICE
            this->setupScriptable( node );
#endif

            return node;
        }
    };
}
