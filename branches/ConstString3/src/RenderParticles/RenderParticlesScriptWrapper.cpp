#	include "RenderParticlesScriptWrapper.h"
#	include "EmitterDescription.h"
#	include "EmitterDescriptionManager.h"
//#	include "pybind/pybind.hpp"
#	include "Menge/ScriptEngine.h"

namespace Menge
{
	EmitterDescription * s_createEmitterDescription()
	{
		EmitterDescription * descr =  
			EmitterDescriptionManager::get()->createEmitterDescription();

		return descr;
	}

	ParticlesAtlas * s_createParticlesAtlas( const String& _file, const String& _path )
	{
		ParticlesAtlas * atlas =  
			EmitterDescriptionManager::get()->createAtlas( _file, _path );

		return atlas;
	}

	void s_notifyLoadEmitter(EmitterDescription * _descr)
	{
		RenderParticlesScriptWrapper::get()->notifyLoadEmitter( _descr );
	}

	void s_notifyLoadParticlesAtlas(ParticlesAtlas * _atlas)
	{
		RenderParticlesScriptWrapper::get()->notifyLoadParticlesAtlas( _atlas );
	}
	
	////////////////////////////////////////////////////////////
	RenderParticlesScriptWrapper::RenderParticlesScriptWrapper()
		: m_scriptEngine(0)
		, m_scriptModule(NULL)
	{
	}	
	////////////////////////////////////////////////////////////
	bool RenderParticlesScriptWrapper::initialise( ScriptServiceInterface * _service, const String & _modulePath )
	{
		m_scriptEngine = _service;
		if( m_scriptEngine == 0 )
		{
			return false;
		}
				
		wrap_();

		TVectorString paths;
		
		paths.push_back( _modulePath );

		m_scriptEngine->addModulePath( paths );

		ConstString startScriptName("RenderParticles");
		m_scriptModule = m_scriptEngine->importModule( startScriptName );

		if( m_scriptModule == 0 )
		{
			return false;	
		}

		m_scriptEngine->setCurrentModule(m_scriptModule);
		
		return true;
	}
	/////////////////////////////////////////////////////////////
	void RenderParticlesScriptWrapper::callFunction( const char * _functionName, const char * _params, ... )
	{
		PyObject * fun = m_scriptEngine->getModuleFunction( m_scriptModule, _functionName );
		
		va_list valist;
		va_start(valist, _params);
		m_scriptEngine->callFunction(fun, _params, valist);
		va_end( valist ); 
	}
	////////////////////////////////////////////////////////////
	void RenderParticlesScriptWrapper::wrap_()
	{
		pybind::class_<EmitterDescription>("EmitterDescription")
			.def( "setName", &EmitterDescription::setName )
			.def( "setLeftBorder", &EmitterDescription::setLeftBorder )
			.def( "setRightBorder", &EmitterDescription::setRightBorder )
			.def( "setDuration", &EmitterDescription::setDuration )
			.def( "setEmitterId", &EmitterDescription::setEmitterId )
			.def( "setSize", &EmitterDescription::setSize )
			.def( "setRelative", &EmitterDescription::setRelative )

			.def( "getName", &EmitterDescription::getName )
			.def( "getLeftBorder", &EmitterDescription::getLeftBorder )
			.def( "getRightBorder", &EmitterDescription::getRightBorder )
			.def( "getDuration", &EmitterDescription::getDuration )
			.def( "getRelative", &EmitterDescription::getRelative )
			;
		pybind::class_<ParticlesAtlas>("ParticlesAtlas")
			.def( pybind::init<const String &,const String &>() )
			;

		pybind::def_function( "notifyLoadEmitter", &s_notifyLoadEmitter );
		pybind::def_function( "notifyLoadParticlesAtlas", &s_notifyLoadParticlesAtlas );
		pybind::def_function( "createEmitterDescription", &s_createEmitterDescription );
		pybind::def_function( "createParticlesAtlas", &s_createParticlesAtlas );
	}
	////////////////////////////////////////////////////////////
	void RenderParticlesScriptWrapper::addListener( RenderParticlesListener * _listener )
	{
		m_listeners.push_back(_listener);
	}
	///////////////////////////////////////////////////////////
	void RenderParticlesScriptWrapper::removeListener( RenderParticlesListener * _listener )
	{
		TVectorParticlesScriptListener::iterator it = std::remove( m_listeners.begin(), m_listeners.end(), _listener );
		m_listeners.erase(it);
	}
	////////////////////////////////////////////////////////////
	void RenderParticlesScriptWrapper::notifyLoadEmitter( EmitterDescription * _descr )
	{
		for( TVectorParticlesScriptListener::iterator 
			it = m_listeners.begin(),
			it_end = m_listeners.end();
			it != it_end;
			it++
			)
			{
				(*it)->onLoadEmitter( _descr );
			}
	}
	////////////////////////////////////////////////////////////
	void RenderParticlesScriptWrapper::notifyLoadParticlesAtlas( ParticlesAtlas * _atlas )
	{
		for( TVectorParticlesScriptListener::iterator 
			it = m_listeners.begin(),
			it_end = m_listeners.end();
		it != it_end;
		it++
			)
		{
			(*it)->onLoadAtlas( _atlas );
		}
	}
	////////////////////////////////////////////////////////////
}