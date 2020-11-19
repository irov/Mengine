#include "AstralaxScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/ScriptWrapperInterface.h"
#include "Interface/ResourceServiceInterface.h"

#include "Environment/Python/PythonAnimatableEventReceiver.h"
#include "Environment/Python/PythonScriptWrapper.h"

#include "Frameworks/PythonFramework/DocumentTraceback.h"

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
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxScriptEmbedding::embedding( pybind::kernel_interface * _kernel )
    {
        pybind::interface_<ResourceAstralax, pybind::bases<Resource>>( _kernel, "ResourceAstralax", false )
            .def( "getResourceImageCount", &ResourceAstralax::getResourceImageCount )
            .def( "getResourceImage", &ResourceAstralax::getResourceImage )
            ;

        pybind::interface_<AstralaxEmitter, pybind::bases<Node, Eventable, Animatable>>( _kernel, "ParticleEmitter2", false )
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

            .def( "getDuration", &AstralaxEmitter::getDuration )

            .def( "setEmitterRandomMode", &AstralaxEmitter::setEmitterRandomMode )
            .def( "getEmitterRandomMode", &AstralaxEmitter::getEmitterRandomMode )
            .def_static_native_kernel( "setEventListener", &Detail::s_AstralaxEmitter_setEventListener )
            ;

        VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ParticleEmitter2" ), Helper::makeFactorableUnique<PythonScriptWrapper<AstralaxEmitter>>( MENGINE_DOCUMENT_FACTORABLE, _kernel ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "AstralaxEmitter" ), Helper::makeFactorableUnique<PythonScriptWrapper<AstralaxEmitter>>( MENGINE_DOCUMENT_FACTORABLE, _kernel ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ResourceAstralax" ), Helper::makeFactorableUnique<PythonScriptWrapper<ResourceAstralax>>( MENGINE_DOCUMENT_FACTORABLE, _kernel ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ResourceParticle" ), Helper::makeFactorableUnique<PythonScriptWrapper<ResourceAstralax>>( MENGINE_DOCUMENT_FACTORABLE, _kernel ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxScriptEmbedding::ejecting( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<ResourceAstralax>();
        _kernel->remove_scope<AstralaxEmitter>();

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ParticleEmitter2" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "AstralaxEmitter" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ResourceAstralax" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ResourceParticle" ) );
    }
}

