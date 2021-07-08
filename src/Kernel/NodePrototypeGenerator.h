#pragma once

#ifdef MENGINE_USE_SCRIPT_SERVICE
#include "Kernel/ScriptablePrototypeGenerator.h"
#else
#include "Kernel/DefaultPrototypeGenerator.h"
#endif

#include "Interface/EnumeratorServiceInterface.h"

#include "Kernel/Node.h"
#include "Kernel/ConstString.h"
#include "Kernel/Document.h"
#include "Kernel/DocumentHelper.h"
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
            const FactoryPtr & factory = this->getPrototypeFactory();

            NodePtr node = factory->createObject( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( node, "can't generate '%s' '%s' doc '%s'"
                , this->getCategory().c_str()
                , this->getPrototype().c_str()
                , MENGINE_DOCUMENT_STR( _doc )
            );

            UniqueId uniqueIdentity = ENUMERATOR_SERVICE()
                ->generateUniqueIdentity();

            node->setUniqueIdentity( uniqueIdentity );

#if MENGINE_DOCUMENT_ENABLE
            DocumentPtr doc = MENGINE_DOCUMENT_MESSAGE( "Node type '%s' uid '%u'"
                , node->getType().c_str()
                , uniqueIdentity
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
