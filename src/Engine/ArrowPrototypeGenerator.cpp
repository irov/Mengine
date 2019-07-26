#include "ArrowPrototypeGenerator.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/Arrow.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

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
    FactorablePointer ArrowPrototypeGenerator::generate( const Char * _doc )
    {
        ArrowPtr arrow = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Arrow" ), _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( arrow, nullptr, "can't create '%s' '%s' doc '%s'"
            , m_category.c_str()
            , m_prototype.c_str()
            , _doc
        );

        return arrow;
    }
}