#include "FigmaScriptEmbedding.h"

#include "Figma.h"
#include "ResourceFigma.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonScriptWrapper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FigmaScriptEmbedding::FigmaScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FigmaScriptEmbedding::~FigmaScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FigmaScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::interface_<ResourceFigma, pybind::bases<Resource>>( _kernel, "ResourceFigma", false )
            ;

        pybind::interface_<Figma, pybind::bases<Node>>( _kernel, "Figma", false )
            ;

        Helper::registerScriptWrapping<ResourceFigma>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<Figma>( _kernel, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FigmaScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<Figma>();
        _kernel->remove_scope<ResourceFigma>();

        Helper::unregisterScriptWrapping<Figma>();
        Helper::unregisterScriptWrapping<ResourceFigma>();
    }
    //////////////////////////////////////////////////////////////////////////
}
