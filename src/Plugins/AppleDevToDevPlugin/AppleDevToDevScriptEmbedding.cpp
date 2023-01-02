#include "AppleDevToDevScriptEmbedding.h"

#include "AppleDevToDevInterface.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonDocumentTraceback.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Factory.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    namespace Detail
    {

    }
    //////////////////////////////////////////////////////////////////////////
    AppleDevToDevScriptEmbedding::AppleDevToDevScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleDevToDevScriptEmbedding::~AppleDevToDevScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleDevToDevScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        SCRIPT_SERVICE()
            ->setAvailablePlugin( "AppleDevToDev", true );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleDevToDevScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {

    }
    //////////////////////////////////////////////////////////////////////////
}

