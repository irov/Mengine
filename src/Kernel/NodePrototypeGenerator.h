#pragma once

#include "Interface/DocumentInterface.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "Kernel/ScriptablePrototypeGenerator.h"
#else
#   include "Kernel/DefaultPrototypeGenerator.h"
#endif

#include "Kernel/Node.h"
#include "Kernel/ConstString.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/EnumeratorHelper.h"

namespace Mengine
{
    template<class Type, uint32_t Count>
    class NodePrototypeGenerator
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
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
        FactorablePointer generate( const DocumentInterfacePtr & _doc ) override
        {
            const FactoryInterfacePtr & factory = this->getPrototypeFactory();

            NodePtr node = factory->createObject( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( node, "can't generate '%s' '%s' doc '%s'"
                , this->getCategory().c_str()
                , this->getPrototype().c_str()
                , MENGINE_DOCUMENT_STR( _doc )
            );

            UniqueId uniqueIdentity = Helper::generateUniqueIdentity();

            node->setUniqueIdentity( uniqueIdentity );

#if defined(MENGINE_DOCUMENT_ENABLE)
            DocumentInterfacePtr doc = MENGINE_DOCUMENT_MESSAGE( "Node type '%s' uid '%u'"
                , node->getType().c_str()
                , uniqueIdentity
            );

            doc->setParent( _doc );

            node->setDocument( doc );
#endif

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
            this->setupScriptable( node );
#endif

            return node;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class Type, uint32_t Count>
        FactoryPrototypeGeneratorPtr makeNodePrototypeGenerator( const DocumentInterfacePtr & _doc )
        {
            FactoryPrototypeGeneratorPtr generator = Helper::makeFactorableUnique<NodePrototypeGenerator<Type, Count>>( _doc );

            return generator;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
