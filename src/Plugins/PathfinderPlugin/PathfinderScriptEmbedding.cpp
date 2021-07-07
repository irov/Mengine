#include "PathfinderScriptEmbedding.h"

#include "Interface/VocabularyServiceInterface.h"

#include "Environment/Python/PythonEventReceiver.h"
#include "Environment/Python/PythonScriptWrapper.h"
#include "Environment/Python/PythonDocumentTraceback.h"

#include "Kernel/Document.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PathfinderScriptEmbedding::PathfinderScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PathfinderScriptEmbedding::~PathfinderScriptEmbedding()
    {
    }    
    //////////////////////////////////////////////////////////////////////////
    bool PathfinderScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        MENGINE_UNUSED( _kernel );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PathfinderScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        MENGINE_UNUSED( _kernel );
    }
    //////////////////////////////////////////////////////////////////////////
}

