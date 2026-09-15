#include "MosaicScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonScriptWrapper.h"
#include "Environment/Python/PythonDocument.h"

#include "MosaicInterface.h"

#include "MosaicRender.h"

#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MosaicScriptEmbedding::MosaicScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MosaicScriptEmbedding::~MosaicScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::interface_<MosaicRender, pybind::bases<Node>>( _kernel, "MosaicRender", false )
            ;

        Helper::registerScriptWrapping<MosaicRender>( _kernel, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        Helper::unregisterScriptWrapping<MosaicRender>();

        _kernel->remove_scope<MosaicRender>();
    }
    //////////////////////////////////////////////////////////////////////////
}
