#include "OzzScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonAnimatableEventReceiver.h"
#include "Environment/Python/PythonScriptWrapper.h"
#include "Environment/Python/PythonDocumentTraceback.h"

#include "ResourceOzzAnimation.h"
#include "ResourceOzzMesh.h"
#include "ResourceOzzSkeleton.h"

#include "OzzAnimationInterface.h"

#include "NodeOzzAnimation.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static SamplerOzzAnimationPtr s_createOzzSampler()
        {
            SamplerOzzAnimationPtr sampler = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Sampler" ), STRINGIZE_STRING_LOCAL( "SamplerOzzAnimation" ), MENGINE_DOCUMENT_PYBIND );

            MENGINE_ASSERTION_MEMORY_PANIC( sampler );

            return sampler;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_SamplerOzzAnimationInterface_setEventListener( pybind::kernel_interface * _kernel, SamplerOzzAnimationInterface * _sampler, PyObject * _args, PyObject * _kwds )
        {
            MENGINE_UNUSED( _args );

            MENGINE_ASSERTION_MEMORY_PANIC( _kwds );

            pybind::dict py_kwds( _kernel, _kwds );
            Helper::registerAnimatableEventReceiver<>( _kernel, py_kwds, _sampler, MENGINE_DOCUMENT_PYBIND );

            MENGINE_ASSERTION_PYTHON_EVENT_RECEIVER( _sampler, py_kwds );

            return _kernel->ret_none();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    OzzScriptEmbedding::OzzScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OzzScriptEmbedding::~OzzScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool OzzScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        SCRIPT_SERVICE()
            ->setAvailablePlugin( "Ozz", true );

        pybind::def_function( _kernel, "createOzzSampler", &Detail::s_createOzzSampler );

        pybind::interface_<ResourceOzzAnimation, pybind::bases<Resource>>( _kernel, "ResourceOzzAnimation", false )
            ;

        pybind::interface_<ResourceOzzMesh, pybind::bases<Resource>>( _kernel, "ResourceOzzMesh", false )
            ;

        pybind::interface_<ResourceOzzSkeleton, pybind::bases<Resource>>( _kernel, "ResourceOzzSkeleton", false )
            ;

        pybind::interface_<SamplerOzzAnimationInterface, pybind::bases<Identity, Compilable, Animatable, Eventable, Scriptable>>( _kernel, "SamplerOzzAnimationInterface" )
            .def( "setResourceOzzAnimation", &SamplerOzzAnimationInterface::setResourceOzzAnimation )
            .def( "getResourceOzzAnimation", &SamplerOzzAnimationInterface::getResourceOzzAnimation )
            .def( "setResourceOzzSkeleton", &SamplerOzzAnimationInterface::setResourceOzzSkeleton )
            .def( "getResourceOzzSkeleton", &SamplerOzzAnimationInterface::getResourceOzzSkeleton )
            .def( "setWeight", &SamplerOzzAnimationInterface::setWeight )
            .def( "getWeight", &SamplerOzzAnimationInterface::getWeight )
            .def_static_native_kernel( "setEventListener", &Detail::s_SamplerOzzAnimationInterface_setEventListener )
            ;

        pybind::interface_<SamplerOzzAnimation, pybind::bases<SamplerOzzAnimationInterface>>( _kernel, "SamplerOzzAnimation", false )
            ;

        pybind::interface_<NodeOzzAnimation, pybind::bases<Node>>( _kernel, "NodeOzzAnimation", false )
            .def( "setResourceOzzImage", &NodeOzzAnimation::setResourceOzzImage )
            .def( "getResourceOzzImage", &NodeOzzAnimation::getResourceOzzImage )
            .def( "setResourceOzzSkeleton", &NodeOzzAnimation::setResourceOzzSkeleton )
            .def( "getResourceOzzSkeleton", &NodeOzzAnimation::getResourceOzzSkeleton )
            .def( "setResourceOzzMesh", &NodeOzzAnimation::setResourceOzzMesh )
            .def( "getResourceOzzMesh", &NodeOzzAnimation::getResourceOzzMesh )
            .def( "addOzzAnimationSampler", &NodeOzzAnimation::addOzzAnimationSampler )
            .def( "removeOzzAnimationSampler", &NodeOzzAnimation::removeOzzAnimationSampler )
            .def( "findOzzAnimationSampler", &NodeOzzAnimation::findOzzAnimationSampler )
            .def( "getOzzAnimationSamplerCount", &NodeOzzAnimation::getOzzAnimationSamplerCount )
            .def( "getOzzAnimationSampler", &NodeOzzAnimation::getOzzAnimationSampler )
            ;

        Helper::registerScriptWrapping<ResourceOzzAnimation>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<ResourceOzzMesh>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<ResourceOzzSkeleton>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<SamplerOzzAnimation>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<NodeOzzAnimation>( _kernel, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OzzScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<ResourceOzzAnimation>();
        _kernel->remove_scope<ResourceOzzMesh>();
        _kernel->remove_scope<ResourceOzzSkeleton>();
        _kernel->remove_scope<SamplerOzzAnimationInterface>();
        _kernel->remove_scope<NodeOzzAnimation>();

        Helper::unregisterScriptWrapping<ResourceOzzAnimation>();
        Helper::unregisterScriptWrapping<ResourceOzzMesh>();
        Helper::unregisterScriptWrapping<ResourceOzzSkeleton>();
        Helper::unregisterScriptWrapping<SamplerOzzAnimation>();
        Helper::unregisterScriptWrapping<NodeOzzAnimation>();
    }
    //////////////////////////////////////////////////////////////////////////
}
