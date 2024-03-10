#include "SpineScriptEmbedding.h"

#include "Interface/StringizeServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonScriptWrapper.h"
#include "Environment/Python/PythonAnimatableEventReceiver.h"
#include "Environment/Python/PythonDocumentTraceback.h"
#include "Environment/Python/PythonEventReceiver.h"

#include "Spine.h"
#include "ResourceSpineAtlasDefault.h"
#include "ResourceSpineAtlasTexturepacker.h"
#include "ResourceSpineSkeleton.h"

#include "Kernel/Logger.h"
#include "Kernel/Error.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class PythonSpineEventReceiver
            : public PythonAnimatableEventReceiver<SamplerSpineEventReceiverInterface>
        {
        public:
            void onSamplerSpineEvent( const Char * _eventName, int32_t _eventIntValue, float _eventFloatValue, const Char * _eventStringValue ) override
            {
                m_cb.call( _eventName, _eventIntValue, _eventFloatValue, _eventStringValue );
            }

            void onSamplerSpineStateAnimationEnd( const ConstString & _state, const ConstString & _animation, bool _isEnd ) override
            {
                m_cb.call( _state, _animation, _isEnd );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_SamplerSpineAnimationInterface_setEventListener( pybind::kernel_interface * _kernel, SamplerSpineAnimationInterface * _sampler, PyObject * _args, PyObject * _kwds )
        {
            MENGINE_UNUSED( _args );

            MENGINE_ASSERTION_MEMORY_PANIC( _kwds, "invalid set event listener" );

            pybind::dict py_kwds( _kernel, _kwds );
            Helper::registerAnimatableEventReceiver<>( _kernel, py_kwds, _sampler, MENGINE_DOCUMENT_PYBIND );

            Helper::registerPythonEventReceiver<PythonSpineEventReceiver>( _kernel, py_kwds, _sampler, STRINGIZE_STRING_LOCAL( "onSamplerSpineEvent" ), EVENT_SAMPLER_SPINE_EVENT, MENGINE_DOCUMENT_PYBIND );
            Helper::registerPythonEventReceiver<PythonSpineEventReceiver>( _kernel, py_kwds, _sampler, STRINGIZE_STRING_LOCAL( "onSamplerSpineStateAnimationEnd" ), EVENT_SAMPLER_SPINE_STATE_ANIMATION_END, MENGINE_DOCUMENT_PYBIND );

            MENGINE_ASSERTION_PYTHON_EVENT_RECEIVER( _sampler, py_kwds );

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
    bool SpineScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        SCRIPT_SERVICE()
            ->setAvailablePlugin( "Spine", true );

        pybind::interface_<Spine, pybind::bases<Node> >( _kernel, "Spine", false )
            .def( "setResourceSpineSkeleton", &Spine::setResourceSpineSkeleton )
            .def( "getResourceSpineSkeleton", &Spine::getResourceSpineSkeleton )
            .def( "addAnimationSampler", &Spine::addAnimationSampler )
            .def( "removeAnimationSampler", &Spine::removeAnimationSampler )
            .def( "clearAnimationSamplers", &Spine::clearAnimationSamplers )
            .def( "findAnimationSampler", &Spine::findAnimationSampler )
            .def( "getAnimationSamplerCount", &Spine::getAnimationSamplerCount )
            .def( "getAnimationSampler", &Spine::getAnimationSampler )
            ;

        pybind::interface_<SamplerSpineAnimationInterface, pybind::bases<Identity, Compilable, Updatable, Animatable, Eventable, Scriptable>>( _kernel, "SamplerSpineAnimationInterface", false )
            .def( "setResourceSpineSkeleton", &SamplerSpineAnimationInterface::setResourceSpineSkeleton )
            .def( "getResourceSpineSkeleton", &SamplerSpineAnimationInterface::getResourceSpineSkeleton )
            .def( "setAnimationName", &SamplerSpineAnimationInterface::setAnimationName )
            .def( "getAnimationName", &SamplerSpineAnimationInterface::getAnimationName )
            .def( "setAnimationEnable", &SamplerSpineAnimationInterface::setAnimationEnable )
            .def( "getAnimationEnable", &SamplerSpineAnimationInterface::getAnimationEnable )
            .def_static_native_kernel( "setEventListener", &Detail::s_SamplerSpineAnimationInterface_setEventListener )
            ;

        pybind::interface_<SamplerSpineAnimation, pybind::bases<SamplerSpineAnimationInterface>>( _kernel, "SamplerSpineAnimation", false )
            ;

        pybind::interface_<ResourceSpineAtlas, pybind::bases<Resource> >( _kernel, "ResourceSpineAtlas", false )
            ;

        pybind::interface_<ResourceSpineAtlasDefault, pybind::bases<ResourceSpineAtlas>>( _kernel, "ResourceSpineAtlasDefault", false )
            .def( "addResourceImageDesc", &ResourceSpineAtlasDefault::addResourceImageDesc )
            ;

        pybind::interface_<ResourceSpineAtlasTexturepacker, pybind::bases<ResourceSpineAtlas>>( _kernel, "ResourceSpineAtlasTexturepacker", false )
            .def( "addResourceTexturepacker", &ResourceSpineAtlasTexturepacker::addResourceTexturepacker )
            ;

        pybind::interface_<ResourceSpineSkeleton, pybind::bases<Resource>>( _kernel, "ResourceSpineSkeleton", false )
            .def( "setResourceSpineAtlas", &ResourceSpineSkeleton::setResourceSpineAtlas )
            .def( "getResourceSpineAtlas", &ResourceSpineSkeleton::getResourceSpineAtlas )
            ;

        Helper::registerScriptWrapping<Spine>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<SamplerSpineAnimation>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<ResourceSpineSkeleton>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<ResourceSpineAtlasDefault>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<ResourceSpineAtlasTexturepacker>( _kernel, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SpineScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<Spine>();
        _kernel->remove_scope<SamplerSpineAnimation>();
        _kernel->remove_scope<ResourceSpineAtlas>();
        _kernel->remove_scope<ResourceSpineAtlasDefault>();
        _kernel->remove_scope<ResourceSpineAtlasTexturepacker>();
        _kernel->remove_scope<ResourceSpineSkeleton>();

        Helper::unregisterScriptWrapping<Spine>();
        Helper::unregisterScriptWrapping<SamplerSpineAnimation>();
        Helper::unregisterScriptWrapping<ResourceSpineSkeleton>();
        Helper::unregisterScriptWrapping<ResourceSpineAtlasDefault>();
        Helper::unregisterScriptWrapping<ResourceSpineAtlasTexturepacker>();
    }
    //////////////////////////////////////////////////////////////////////////
}
