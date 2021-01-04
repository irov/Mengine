#include "AstralaxScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"

#include "Environment/Python/PythonAnimatableEventReceiver.h"
#include "Environment/Python/PythonScriptWrapper.h"
#include "Environment/Python/PythonDocumentTraceback.h"

#include "ResourceAstralax.h"
#include "AstralaxEmitter.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

#include "pybind/pybind.hpp"
#include "pybind/stl/stl_type_cast.hpp"

namespace Mengine
{
    namespace Detail
    {
        ////////////////////////////////////////////////////////////////////////////        
        static PyObject * s_AstralaxEmitter_setEventListener( pybind::kernel_interface * _kernel, AstralaxEmitter * _node, PyObject * _args, PyObject * _kwds )
        {
            MENGINE_UNUSED( _args );

            MENGINE_ASSERTION_MEMORY_PANIC( _kwds );

            pybind::dict py_kwds( _kernel, _kwds );
            Helper::registerAnimatableEventReceiver<>( _kernel, py_kwds, _node, MENGINE_DOCUMENT_PYBIND );

            MENGINE_ASSERTION_PYTHON_EVENT_RECEIVER( _node, py_kwds );

            return _kernel->ret_none();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    AstralaxScriptEmbedding::AstralaxScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AstralaxScriptEmbedding::~AstralaxScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void s_AstralaxEmitter_changeEmitterImage( pybind::kernel_interface * _kernel, AstralaxEmitter * _emitter, const ConstString & _emitterImageName )
        {
            MENGINE_UNUSED( _kernel );

            const ResourcePtr & resource = RESOURCE_SERVICE()
                ->getResourceReference( ConstString::none(), _emitterImageName );

            MENGINE_ASSERTION_MEMORY_PANIC( resource, "emitter '%s' can't get resource '%s'"
                , _emitter->getName().c_str()
                , _emitterImageName.c_str()
            );

            _emitter->changeEmitterImage( resource );
        }
        //////////////////////////////////////////////////////////////////////////
        static float s_AstralaxEmitter_getDuration( AstralaxEmitter * _emitter )
        {
            const AnimationInterface * animation = _emitter->getAnimation();

            float duration = animation->getDuration();

            return duration;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxScriptEmbedding::embedding( pybind::kernel_interface * _kernel )
    {
        pybind::interface_<ResourceAstralax, pybind::bases<Resource>>( _kernel, "ResourceAstralax", false )
            .def( "getResourceImageCount", &ResourceAstralax::getResourceImageCount )
            .def( "getResourceImage", &ResourceAstralax::getResourceImage )
            ;

        pybind::interface_<AstralaxEmitter, pybind::bases<Node>>( _kernel, "ParticleEmitter2", false )
            .def_deprecated( "setResourceParticle", &AstralaxEmitter::setResourceAstralax, "use setResourceAstralax" )
            .def_deprecated( "getResourceParticle", &AstralaxEmitter::getResourceAstralax, "use getResourceAstralax" )
            .def( "setResourceAstralax", &AstralaxEmitter::setResourceAstralax )
            .def( "getResourceAstralax", &AstralaxEmitter::getResourceAstralax )

            .def( "setEmitterTranslateWithParticle", &AstralaxEmitter::setEmitterTranslateWithParticle )
            .def( "setEmitterPositionRelative", &AstralaxEmitter::setEmitterPositionRelative )
            .def( "setEmitterCameraRelative", &AstralaxEmitter::setEmitterCameraRelative )
            .def( "setEmitterPositionProviderOriginOffset", &AstralaxEmitter::setEmitterPositionProviderOriginOffset )

            .def_static_kernel( "changeEmitterImage", &Detail::s_AstralaxEmitter_changeEmitterImage )
            .def( "removeEmitterImage", &AstralaxEmitter::removeEmitterImage )
            .def( "changeEmitterPolygon", &AstralaxEmitter::changeEmitterPolygon )
            .def( "removeEmitterPolygon", &AstralaxEmitter::removeEmitterPolygon )

            .def_static_deprecated( "getDuration", &Detail::s_AstralaxEmitter_getDuration, "use getAnimation" )

            .def( "setEmitterRandomMode", &AstralaxEmitter::setEmitterRandomMode )
            .def( "getEmitterRandomMode", &AstralaxEmitter::getEmitterRandomMode )
            .def_static_native_kernel( "setEventListener", &Detail::s_AstralaxEmitter_setEventListener )
            ;

        Helper::registerScriptWrapping<AstralaxEmitter>( _kernel, STRINGIZE_STRING_LOCAL( "ParticleEmitter2" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<AstralaxEmitter>( _kernel, STRINGIZE_STRING_LOCAL( "AstralaxEmitter" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<ResourceAstralax>( _kernel, STRINGIZE_STRING_LOCAL( "ResourceAstralax" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<ResourceAstralax>( _kernel, STRINGIZE_STRING_LOCAL( "ResourceParticle" ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxScriptEmbedding::ejecting( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<ResourceAstralax>();
        _kernel->remove_scope<AstralaxEmitter>();

        Helper::unregisterScriptWrapping( STRINGIZE_STRING_LOCAL( "ParticleEmitter2" ) );
        Helper::unregisterScriptWrapping( STRINGIZE_STRING_LOCAL( "AstralaxEmitter" ) );
        Helper::unregisterScriptWrapping( STRINGIZE_STRING_LOCAL( "ResourceAstralax" ) );
        Helper::unregisterScriptWrapping( STRINGIZE_STRING_LOCAL( "ResourceParticle" ) );
    }
}

