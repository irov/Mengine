#include "ArrowPrototypeGenerator.h"

#include "Kernel/Arrow.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/PrototypeHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ArrowPrototypeGenerator::ArrowPrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ArrowPrototypeGenerator::~ArrowPrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FactorablePointer ArrowPrototypeGenerator::generate( const DocumentInterfacePtr & _doc )
    {
        ArrowInterfacePtr arrow = Helper::makeFactorableUnique<Arrow>( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( arrow, "can't create '%s' '%s' doc '%s'"
            , m_category.c_str()
            , m_prototype.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        return arrow;
    }
    //////////////////////////////////////////////////////////////////////////
}