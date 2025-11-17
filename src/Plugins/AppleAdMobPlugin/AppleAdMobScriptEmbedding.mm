#include "AppleAdMobScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocument.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleAdMobScriptEmbedding::AppleAdMobScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleAdMobScriptEmbedding::~AppleAdMobScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleAdMobScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        // No functions to export to script
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAdMobScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        // No functions to remove from script
    }
    //////////////////////////////////////////////////////////////////////////
}

