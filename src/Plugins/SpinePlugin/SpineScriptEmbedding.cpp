#include "SpineScriptEmbedding.h"

#include "Interface/VocabularyServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"

#include "Environment/Python/PythonScriptWrapper.h"
#include "Environment/Python/PythonAnimatableEventReceiver.h"

#include "Spine.h"
#include "ResourceSpineAtlasDefault.h"
#include "ResourceSpineAtlasTexturepacker.h"
#include "ResourceSpineSkeleton.h"

#include "Kernel/Logger.h"
#include "Kernel/Error.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class PythonSpineEventReceiver
            : public PythonAnimatableEventReceiver<SpineEventReceiverInterface>
        {
        public:
            void onSpineEvent( const Char * _eventName, int32_t _eventIntValue, float _eventFloatValue, const Char * _eventStringValue ) override
            {
                m_cb.call( _eventName, _eventIntValue, _eventFloatValue, _eventStringValue );
            }

            void onSpineStateAnimationEnd( const ConstString & _state, const ConstString & _animation, bool _isEnd ) override
            {
                m_cb.call( _state, _animation, _isEnd );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_Spine_setEventListener( pybind::kernel_interface * _kernel, Spine * _node, PyObject * _args, PyObject * _kwds )
        {
            MENGINE_UNUSED( _args );

            MENGINE_ASSERTION_MEMORY_PANIC( _kwds, _kernel->ret_none(), "invalid set event listener" );

            pybind::dict py_kwds( _kernel, _kwds );
            Helper::registerAnimatableEventReceiver<>( _kernel, py_kwds, _node );

            Helper::registerPythonEventReceiver<PythonSpineEventReceiver>( _kernel, py_kwds, _node, "onSpineEvent", EVENT_SPINE_EVENT );
            Helper::registerPythonEventReceiver<PythonSpineEventReceiver>( _kernel, py_kwds, _node, "onSpineStateAnimationEnd", EVENT_SPINE_STATE_ANIMATION_END );

            MENGINE_ASSERTION_PYTHON_EVENT_RECEIVER( _node, py_kwds );

            return _kernel->ret_none();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    SpineScriptEmbedding::SpineScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SpineScriptEmbedding::~SpineScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SpineScriptEmbedding::embedding( pybind::kernel_interface * _kernel )
    {
        pybind::interface_<Spine, pybind::bases<Node, Eventable, Animatable> >( _kernel, "Spine", false )
            .def( "setResourceSpineSkeleton", &Spine::setResourceSpineSkeleton )
            .def( "getResourceSpineSkeleton", &Spine::getResourceSpineSkeleton )
            .def( "mixAnimation", &Spine::mixAnimation )
            .def( "getAnimationDuration", &Spine::getAnimationDuration )
            .def( "setStateAnimation", &Spine::setStateAnimation )
            .def( "removeStateAnimation", &Spine::removeStateAnimation )
            .def( "setStateAnimationSpeedFactor", &Spine::setStateAnimationSpeedFactor )
            .def( "getStateAnimationSpeedFactor", &Spine::getStateAnimationSpeedFactor )
            .def( "setStateAnimationTiming", &Spine::setStateAnimationTiming )
            .def( "getStateAnimationTiming", &Spine::getStateAnimationTiming )
            .def( "setStateAnimationFreeze", &Spine::setStateAnimationFreeze )
            .def( "getStateAnimationFreeze", &Spine::getStateAnimationFreeze )
            .def( "getStateAnimationDuration", &Spine::getStateAnimationDuration )
            .def_static_native_kernel( "setEventListener", &Detail::s_Spine_setEventListener )
            ;

        pybind::interface_<ResourceSpineAtlas, pybind::bases<Resource> >( _kernel, "ResourceSpineAtlas", false )
            ;

        pybind::interface_<ResourceSpineAtlasDefault, pybind::bases<ResourceSpineAtlas>>( _kernel, "ResourceSpineAtlasDefault", false )
            .def( "addResourceImageDesc", &ResourceSpineAtlasDefault::addResourceImageDesc )
            ;

        pybind::interface_<ResourceSpineAtlasTexturepacker, pybind::bases<ResourceSpineAtlas>>( _kernel, "ResourceSpineAtlasTexturepacker", false )
            ;

        pybind::interface_<ResourceSpineSkeleton, pybind::bases<Resource>>( _kernel, "ResourceSpineSkeleton", false )
            .def( "setResourceSpineAtlasName", &ResourceSpineSkeleton::setResourceSpineAtlasName )
            .def( "getResourceSpineAtlasName", &ResourceSpineSkeleton::getResourceSpineAtlasName )
            ;

        VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Spine" ), Helper::makeFactorableUnique<PythonScriptWrapper<Spine> >( _kernel ) );
        VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ResourceSpineSkeleton" ), Helper::makeFactorableUnique<PythonScriptWrapper<ResourceSpineSkeleton> >( _kernel ) );
        VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ResourceSpineAtlasDefault" ), Helper::makeFactorableUnique<PythonScriptWrapper<ResourceSpineAtlasDefault> >( _kernel ) );
        VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ResourceSpineAtlasTexturepacker" ), Helper::makeFactorableUnique<PythonScriptWrapper<ResourceSpineAtlasTexturepacker> >( _kernel ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SpineScriptEmbedding::ejecting( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<Spine>();
        _kernel->remove_scope<ResourceSpineAtlas>();
        _kernel->remove_scope<ResourceSpineAtlasDefault>();
        _kernel->remove_scope<ResourceSpineAtlasTexturepacker>();
        _kernel->remove_scope<ResourceSpineSkeleton>();

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Spine" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ResourceSpineSkeleton" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ResourceSpineAtlasDefault" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ResourceSpineAtlasTexturepacker" ) );
    }
}
