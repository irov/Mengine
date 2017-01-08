#	include "ModuleAreaOfInterest.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/PlayerInterface.h"

#	include "NodeAOITrigger.h"
#	include "NodeAOIActor.h"

#	include "PythonScriptWrapper/ScriptClassWrapper.h"
#	include "PythonScriptWrapper/PythonEventReceiver.h"

#	include "Kernel/NodePrototypeGenerator.h"

#	include <algorithm>

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    class PythonNodeAOITriggerEventReceiver
        : public PythonEventReceiver
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
    PyObject * s_NodeAOITrigger_setEventListener( NodeAOITrigger * _node, PyObject * _args, PyObject * _kwds )
    {
        if( _kwds == nullptr )
        {
            return pybind::ret_none();
        }

        pybind::dict py_kwds( _kwds );

        Helper::registerEventReceiver<PythonNodeAOITriggerEventReceiver>( py_kwds, _node, "onTriggerEnter", EVENT_NODE_AOI_TRIGGER_ENTER );
        Helper::registerEventReceiver<PythonNodeAOITriggerEventReceiver>( py_kwds, _node, "onTriggerLeave", EVENT_NODE_AOI_TRIGGER_LEAVE );

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
		pybind::interface_<AreaOfInterest>("AreaOfInterest")
			.def( "freeze", &AreaOfInterest::freeze )
			.def( "isFreeze", &AreaOfInterest::isFreeze )
			;

		pybind::def_functor( "createAOI", this, &ModuleAreaOfInterest::createAOI );
		pybind::def_functor( "removeAOI", this, &ModuleAreaOfInterest::removeAOI );

		pybind::interface_<NodeAOITrigger, pybind::bases<Node, Eventable> >( "NodeAOITrigger", false )
			.def( "setRadius", &NodeAOITrigger::setRadius )
			.def( "getRadius", &NodeAOITrigger::getRadius )
			.def( "setIFF", &NodeAOITrigger::setIFF )
			.def( "getIFF", &NodeAOITrigger::getIFF )
			.def( "setAOI", &NodeAOITrigger::setAOI )
			.def( "getAOI", &NodeAOITrigger::getAOI )
			.def( "setTriggerUserData", &NodeAOITrigger::setTriggerUserData )
			.def( "getTriggerUserData", &NodeAOITrigger::getTriggerUserData )
            .def_static_native( "setEventListener", &s_NodeAOITrigger_setEventListener )
			;

		pybind::interface_<NodeAOIActor, pybind::bases<Node> >( "NodeAOIActor", false )
			.def( "setRadius", &NodeAOIActor::setRadius )
			.def( "getRadius", &NodeAOIActor::getRadius )
			.def( "setIFF", &NodeAOIActor::setIFF )
			.def( "getIFF", &NodeAOIActor::getIFF )
			.def( "setAOI", &NodeAOIActor::setAOI )
			.def( "getAOI", &NodeAOIActor::getAOI )
			.def( "setActorUserData", &NodeAOIActor::setActorUserData )
			.def( "getActorUserData", &NodeAOIActor::getActorUserData )
			;

		SCRIPT_SERVICE(m_serviceProvider)
			->setWrapper( Helper::stringizeString( m_serviceProvider, "NodeAOITrigger" ), new ClassScriptWrapper<NodeAOITrigger>() );

		SCRIPT_SERVICE( m_serviceProvider )
			->setWrapper( Helper::stringizeString( m_serviceProvider, "NodeAOIActor" ), new ClassScriptWrapper<NodeAOIActor>() );
		
		PROTOTYPE_SERVICE(m_serviceProvider)
			->addPrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Node" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "NodeAOITrigger" ), new NodePrototypeGenerator<NodeAOITrigger, 32> );

		PROTOTYPE_SERVICE( m_serviceProvider )
			->addPrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Node" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "NodeAOIActor" ), new NodePrototypeGenerator<NodeAOIActor, 32> );

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
	void ModuleAreaOfInterest::_render( const RenderObjectState * _state, unsigned int _debugMask )
	{
		(void)_state;
		(void)_debugMask;
		//ToDo
	}
}