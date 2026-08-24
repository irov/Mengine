#include "DazzleScriptEmbedding.h"

#include "DazzleEffect.h"
#include "ResourceDazzleEffect.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonScriptWrapper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DazzleScriptEmbedding::DazzleScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DazzleScriptEmbedding::~DazzleScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzleScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::interface_<ResourceDazzleEffect, pybind::bases<Resource>>( _kernel, "ResourceDazzleEffect", false )
            .def( "setResourceEmitterImage", &ResourceDazzleEffect::setResourceEmitterImage )
            .def( "getResourceEmitterImage", &ResourceDazzleEffect::getResourceEmitterImage )
            ;

        pybind::interface_<DazzleEffect, pybind::bases<Node>>( _kernel, "DazzleEffect", false )
            .def( "setResourceDazzle", &DazzleEffect::setResourceDazzle )
            .def( "getResourceDazzle", &DazzleEffect::getResourceDazzle )
            .def( "setResourceImage", &DazzleEffect::setResourceImage )
            .def( "getResourceImage", &DazzleEffect::getResourceImage )
            .def( "setPathPoints", &DazzleEffect::setPathPoints )
            .def( "setPathPointsColored", &DazzleEffect::setPathPointsColored )
            .def( "setPathPointsColoredGradient", &DazzleEffect::setPathPointsColoredGradient )
            .def( "clearPathPoints", &DazzleEffect::clearPathPoints )
            .def( "primePath", &DazzleEffect::primePath )
            .def( "restartEffect", &DazzleEffect::restartEffect )
            ;

        Helper::registerScriptWrapping<ResourceDazzleEffect>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<DazzleEffect>( _kernel, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<DazzleEffect>();
        _kernel->remove_scope<ResourceDazzleEffect>();

        Helper::unregisterScriptWrapping<DazzleEffect>();
        Helper::unregisterScriptWrapping<ResourceDazzleEffect>();
    }
    //////////////////////////////////////////////////////////////////////////
}
