#	pragma once

#	include "ScriptEngine.h"

namespace Menge
{
	class Scriptable;

	class ScriptRegistratorImpl
	{
		typedef Scriptable * (*FScriptRegistration)();

	public:
		void registration( FScriptRegistration _func );
	}

	template<class T>
	class ScriptRegistrator
		: public ScriptRegistratorImpl
		, public Scriptable
	{
	private:
		const static struct AutoRegistrator
		{
			AutoRegistrator()
			{
				ScriptEngine::registration( gen );
			}

			static Scriptable * gen()
			{
				return new T;
			}
		}s_autoRegistrator;
	};
}