#	include "ModuleAreaOfInterest.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/PlayerInterface.h"

#	include "NodeAOITrigger.h"
#	include "NodeAOIActor.h"

#   include "Kernel/ScriptWrapper.h"
#	include "Kernel/ScriptEventReceiver.h"

#	include "Kernel/NodePrototypeGenerator.h"

#	include <algorithm>

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    class PythonNodeAOITriggerEventReceiver
        : public ScriptEventReceiver
        , public NodeAOITriggerEventReceiver
    {
    public:
        void onNodeAOITriggerEnter( AOIActorProviderInterface * _enemy, uint32_t _iff1, uint32_t _iff2 )
        {
            NodeAOIActor * enemy = static_cast<NodeAOIActor *>(_enemy);

            m_cb.call( enemy, _iff1, _iff2 );
        }

        void onNodeAOITriggerLeave( AOIActorProviderInterface * _enemy, uint32_t _iff1, uint32_t _iff2 )
        {
            NodeAOIActor * enemy = static_cast<NodeAOIActor *>(_enemy);

            m_cb.call( enemy, _iff1, _iff2 );
        }
    };
    //////////////////////////////////////////////////////////////////////////
    PyObject * s_NodeAOITrigger_setEventListener( pybind::kernel_interface * _kernel, NodeAOITrigger * _node, PyObject * _args, PyObject * _kwds )
    {
        (void)_args;

        if( _kwds == nullptr )
        {
            return pybind::ret_none();
        }

        pybind::dict py_kwds( _kernel, _kwds );

        Helper::registerScriptEventReceiver<PythonNodeAOITriggerEventReceiver>( py_kwds, _node, "onTriggerEnter", EVENT_NODE_AOI_TRIGGER_ENTER );
        Helper::registerScriptEventReceiver<PythonNodeAOITriggerEventReceiver>( py_kwds, _node, "onTriggerLeave", EVENT_NODE_AOI_TRIGGER_LEAVE );

        return pybind::ret_none();
    }
	//////////////////////////////////////////////////////////////////////////
	ModuleAreaOfInterest::ModuleAreaOfInterest()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ModuleAreaOfInterest::~ModuleAreaOfInterest()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ModuleAreaOfInterest::_initialize()
	{
		pybind::kernel_interface * kernel = pybind::get_kernel();

		pybind::interface_<AreaOfInterest>( kernel, "AreaOfInterest" )
			.def( "freeze", &AreaOfInterest::freeze )
			.def( "isFreeze", &AreaOfInterest::isFreeze )
			;

		pybind::def_functor( kernel, "createAOI", this, &ModuleAreaOfInterest::createAOI );
		pybind::def_functor( kernel, "removeAOI", this, &ModuleAreaOfInterest::removeAOI );

		pybind::interface_<NodeAOITrigger, pybind::bases<Node, Eventable> >( kernel, "NodeAOITrigger", false )
			.def( "setRadius", &NodeAOITrigger::setRadius )
			.def( "getRadius", &NodeAOITrigger::getRadius )
			.def( "setIFF", &NodeAOITrigger::setIFF )
			.def( "getIFF", &NodeAOITrigger::getIFF )
			.def( "setAOI", &NodeAOITrigger::setAOI )
			.def( "getAOI", &NodeAOITrigger::getAOI )
			.def( "setTriggerUserData", &NodeAOITrigger::setTriggerUserData )
			.def( "getTriggerUserData", &NodeAOITrigger::getTriggerUserData )
            .def_static_native_kernel( "setEventListener", &s_NodeAOITrigger_setEventListener )
			;

		pybind::interface_<NodeAOIActor, pybind::bases<Node> >( kernel, "NodeAOIActor", false )
			.def( "setRadius", &NodeAOIActor::setRadius )
			.def( "getRadius", &NodeAOIActor::getRadius )
			.def( "setIFF", &NodeAOIActor::setIFF )
			.def( "getIFF", &NodeAOIActor::getIFF )
			.def( "setAOI", &NodeAOIActor::setAOI )
			.def( "getAOI", &NodeAOIActor::getAOI )
			.def( "setActorUserData", &NodeAOIActor::setActorUserData )
			.def( "getActorUserData", &NodeAOIActor::getActorUserData )
			;

		SCRIPT_SERVICE()
			->setWrapper( Helper::stringizeString( "NodeAOITrigger" ), new ScriptWrapper<NodeAOITrigger>() );

		SCRIPT_SERVICE()
			->setWrapper( Helper::stringizeString( "NodeAOIActor" ), new ScriptWrapper<NodeAOIActor>() );
		
		PROTOTYPE_SERVICE()
			->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "NodeAOITrigger" ), new NodePrototypeGenerator<NodeAOITrigger, 32> );

		PROTOTYPE_SERVICE()
			->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "NodeAOIActor" ), new NodePrototypeGenerator<NodeAOIActor, 32> );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleAreaOfInterest::_finalize()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	AreaOfInterest * ModuleAreaOfInterest::createAOI()
	{
		AreaOfInterest * aoi = new AreaOfInterest;
        
		if( aoi->initialize() == false )
		{
			return nullptr;
		}

		m_aois.push_back( aoi );

		return aoi;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleAreaOfInterest::removeAOI( AreaOfInterest * _aoi )
	{
		TVectorAreaOfInterest::iterator it_erase = std::find( m_aois.begin(), m_aois.end(), _aoi );

		if( it_erase == m_aois.end() )
		{
			return;
		}

		*it_erase = m_aois.back();
		m_aois.pop_back();

		delete _aoi;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleAreaOfInterest::_tick( float _time, float _timing )
	{
		(void)_time;
		(void)_timing;

		for( TVectorAreaOfInterest::iterator 
			it = m_aois.begin(),
			it_end = m_aois.end();
		it != it_end;
		++it )
		{
			AreaOfInterest * aoi = *it;

			aoi->update();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleAreaOfInterest::_render( const RenderObjectState * _state, uint32_t _debugMask )
	{
		(void)_state;
		(void)_debugMask;
		//ToDo
	}
}