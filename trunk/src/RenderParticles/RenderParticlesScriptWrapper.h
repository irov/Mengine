#	pragma once
//#	include "pybind/pybind.hpp"
#	include "RenderParticlesSuites.h"
#	include "Utils/Core/Holder.h"
#	include "Interface/ScriptSystemInterface.h"
namespace Menge
{
	class ScriptEngine;

	class RenderParticlesScriptWrapper
		:public Holder<RenderParticlesScriptWrapper>
	{
	public:
		RenderParticlesScriptWrapper();
		bool initialise( ScriptServiceInterface * _service, const WString & _modulePath );
		void callFunction( const char * _functionName, const char * _params, ... );
		void addListener( RenderParticlesListener * _listener );
		void removeListener( RenderParticlesListener * _listener );
		
		void notifyLoadEmitter( EmitterDescription * _descr );
		void notifyLoadParticlesAtlas( ParticlesAtlas * _atlas );
	protected:
		void wrap_();
		ScriptServiceInterface * m_scriptEngine;
		PyObject * m_scriptModule;

		typedef std::vector< RenderParticlesListener * > TVectorParticlesScriptListener;
		
		TVectorParticlesScriptListener m_listeners;
	};
}









