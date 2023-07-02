#include "ScenePrototypeGenerator.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/Scene.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ScenePrototypeGenerator::ScenePrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ScenePrototypeGenerator::~ScenePrototypeGenerator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FactorablePointer ScenePrototypeGenerator::generate( const DocumentInterfacePtr & _doc )
    {
        ScenePtr scene = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Scene" ), _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( scene, "can't create '%s' '%s' doc '%s'"
            , m_category.c_str()
            , m_prototype.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        return scene;
    }
    //////////////////////////////////////////////////////////////////////////
}