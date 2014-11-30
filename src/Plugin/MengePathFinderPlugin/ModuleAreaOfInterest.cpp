#	include "ModuleAreaOfInterest.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/PlayerInterface.h"

#	include "Trigger.h"

#	include "Kernel/ScriptClassWrapper.h"
#	include "Kernel/NodePrototypeGenerator.h"

#	include "pybind/pybind.hpp"

#	include <algorithm>

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ModuleAreaOfInterest::ModuleAreaOfInterest()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ModuleAreaOfInterest::~ModuleAreaOfInterest()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleAreaOfInterest::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * ModuleAreaOfInterest::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ModuleAreaOfInterest::initialize()
	{
		pybind::interface_<AreaOfInterest>("AreaOfInterest")
			;

		pybind::def_functor( "createAOI", this, &ModuleAreaOfInterest::createAOI );
		pybind::def_functor( "removeAOI", this, &ModuleAreaOfInterest::removeAOI );

		pybind::interface_<Trigger, pybind::bases<Node> >("Trigger", false)
			.def( "setRadius", &Trigger::setRadius )
			.def( "getRadius", &Trigger::getRadius )
			.def( "setAOI", &Trigger::setAOI )
			.def( "getAOI", &Trigger::getAOI )
			.def( "setUserData", &Trigger::setUserData )
			.def( "getUserData", &Trigger::getUserData )			
			;

		SCRIPT_SERVICE(m_serviceProvider)
			->addWrapping( Helper::stringizeString(m_serviceProvider, "Trigger"), new ScriptClassWrapper<Trigger>() );
		
		PROTOTYPE_SERVICE(m_serviceProvider)
			->addPrototype( STRINGIZE_STRING_LOCAL(m_serviceProvider, "Node"), STRINGIZE_STRING_LOCAL(m_serviceProvider, "Trigger"), new NodePrototypeGenerator<Trigger, 128>(m_serviceProvider) );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleAreaOfInterest::finalize()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ModuleAreaOfInterest::setName( const ConstString & _name )
	{
		m_name = _name;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ModuleAreaOfInterest::getName() const
	{
		return m_name;
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
	void ModuleAreaOfInterest::update( float _time, float _timing )
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
	void ModuleAreaOfInterest::render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask )
	{
		//ToDo
	}
}