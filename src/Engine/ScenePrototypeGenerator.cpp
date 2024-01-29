#include "ScenePrototypeGenerator.h"

#include "Kernel/Scene.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/PrototypeHelper.h"

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
        ScenePtr scene = Helper::generateFactorable<Node, Scene>( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( scene, "can't create '%s' '%s' doc '%s'"
            , m_category.c_str()
            , m_prototype.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        return scene;
    }
    //////////////////////////////////////////////////////////////////////////
}