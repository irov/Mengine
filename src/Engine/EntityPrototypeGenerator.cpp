#include "EntityPrototypeGenerator.h"

#include "Kernel/Entity.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/PrototypeHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    EntityPrototypeGenerator::EntityPrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EntityPrototypeGenerator::~EntityPrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FactorablePointer EntityPrototypeGenerator::generate( const DocumentInterfacePtr & _doc )
    {
        EntityPtr entity = Helper::generateFactorable<Node, Entity>( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( entity, "can't create category '%s' prototype '%s' doc '%s'"
            , m_category.c_str()
            , m_prototype.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        return entity;
    }
    //////////////////////////////////////////////////////////////////////////
}