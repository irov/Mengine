#include "ImGUIScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/PluginServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonAnimatableEventReceiver.h"
#include "Environment/Python/PythonScriptWrapper.h"
#include "Environment/Python/PythonDocument.h"

#include "ImGUIInterface.h"

#include "ImGUIRender.h"
#include "ImGUIScriptRender.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ImGUIScriptEmbedding::ImGUIScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ImGUIScriptEmbedding::~ImGUIScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImGUIScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::interface_<ImGUIScriptRender, pybind::bases<Factorable>>( _kernel, "ImGUIScriptRender", false )
            ;

        pybind::interface_<ImGUIRender, pybind::bases<Node>>( _kernel, "ImGUIRender", false )
            ;

        Helper::registerScriptWrapping<ImGUIRender>( _kernel, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        Helper::unregisterScriptWrapping<ImGUIRender>();

        _kernel->remove_scope<ImGUIRender>();
        _kernel->remove_scope<ImGUIScriptRender>();
    }
    //////////////////////////////////////////////////////////////////////////
}
