#	include "Scriptable.h"

#	include "ScriptEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Scriptable::Scriptable()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	Scriptable::~Scriptable()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	Scriptable::AutoRegistrator::AutoRegistrator( FScriptRegistration _func )
	{
		ScriptEngine::registration( _func );
	}
}