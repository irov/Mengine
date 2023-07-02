#include "EntityPrototypeGenerator.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/Entity.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

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
        EntityPtr entity = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Entity" ), _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( entity, "can't create '%s' '%s' doc '%s'"
            , m_category.c_str()
            , m_prototype.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        return entity;
    }
    //////////////////////////////////////////////////////////////////////////
}