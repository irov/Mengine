#	include "SpinePlugin.h"

#	include "Interface/StringizeInterface.h"
#	include "Interface/ScriptSystemInterface.h"

#	include "PythonScriptWrapper/ScriptClassWrapper.h"
#	include "PythonScriptWrapper/PythonAnimatableEventReceiver.h"

#	include "Kernel/NodePrototypeGenerator.h"
#	include "Kernel/ResourcePrototypeGenerator.h"

#	include "Spine.h"
#	include "ResourceSpine.h"

#	include "pybind/pybind.hpp"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Spine, Mengine::SpinePlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonSpineEventReceiver
        : public PythonAnimatableEventReceiver<SpineEventReceiver>
    {
    public:
        void onSpineEvent( const char * _eventName, int _eventIntValue, float _eventFloatValue, const char * _eventStringValue ) override
        {
            m_cb.call( _eventName, _eventIntValue, _eventFloatValue, _eventStringValue );
        }

        void onSpineStateAnimationEnd( const ConstString & _state, const ConstString & _animation, bool _isEnd ) override
        {
            m_cb.call( _state, _animation, _isEnd );
        }
    };
    //////////////////////////////////////////////////////////////////////////
    static PyObject * s_Spine_setEventListener( pybind::kernel_interface * _kernel, Spine * _spine, PyObject * _args, PyObject * _kwds )
    {
        (void)_args;

        if( _kwds == nullptr )
        {
            return pybind::ret_none();
        }

        pybind::dict py_kwds( _kernel, _kwds );
        Helper::registerAnimatableEventReceiver<PythonSpineEventReceiver>( py_kwds, _spine );
        
        Helper::registerEventReceiver<PythonSpineEventReceiver>( py_kwds, _spine, "onSpineEvent", EVENT_SPINE_EVENT );
        Helper::registerEventReceiver<PythonSpineEventReceiver>( py_kwds, _spine, "onSpineStateAnimationEnd", EVENT_SPINE_STATE_ANIMATION_END );

        return pybind::ret_none();
    }
	//////////////////////////////////////////////////////////////////////////
	SpinePlugin::SpinePlugin()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    SpinePlugin::~SpinePlugin()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	bool SpinePlugin::_initialize()
	{
		pybind::kernel_interface * kernel = pybind::get_kernel();

		pybind::interface_<Spine, pybind::bases<Node, Eventable, Animatable> >( kernel, "Spine", false )
			.def( "setResourceSpine", &Spine::setResourceSpine )
			.def( "getResourceSpine", &Spine::getResourceSpine )
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
            .def_static_native_kernel( "setEventListener", &s_Spine_setEventListener )
			;

		pybind::interface_<ResourceSpine, pybind::bases<ResourceReference> >( kernel, "ResourceSpine", false )
			;

		SCRIPT_SERVICE( m_serviceProvider )
			->setWrapper( Helper::stringizeString( m_serviceProvider, "Spine" ), new ClassScriptWrapper<Spine>() );

		SCRIPT_SERVICE( m_serviceProvider )
			->setWrapper( Helper::stringizeString( m_serviceProvider, "ResourceSpine" ), new ClassScriptWrapper<ResourceSpine>() );

		PROTOTYPE_SERVICE( m_serviceProvider )
			->addPrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Node" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "Spine" ), new NodePrototypeGenerator<Spine, 128> );

		PROTOTYPE_SERVICE( m_serviceProvider )
			->addPrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Resource" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "ResourceSpine" ), new ResourcePrototypeGenerator<ResourceSpine, 128> );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SpinePlugin::_finalize()
	{
	}
}