#include "ImGUIScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonAnimatableEventReceiver.h"
#include "Environment/Python/PythonScriptWrapper.h"
#include "Environment/Python/PythonDocumentTraceback.h"

#include "ImGUIInterface.h"

#include "ImGUIScriptRender.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    namespace Detail
    {
        ////////////////////////////////////////////////////////////////////////////        
        ////////////////////////////////////////////////////////////////////////////
    }
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
        SCRIPT_SERVICE()
            ->setAvailablePlugin( "ImGUI", true );

        pybind::interface_<ImGUIScriptRender, pybind::bases<Factorable>>( _kernel, "ImGUIScriptRender", false )
            ;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<ImGUIScriptRender>();
    }
    //////////////////////////////////////////////////////////////////////////
}