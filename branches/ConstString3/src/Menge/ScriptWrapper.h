#	pragma once

//#	include <xxbind/xxbind.hpp>

namespace Menge
{
	class ScriptWrapper
	{
	public:
		static void actorWrap();
		static void entityWrap();
		static void helperWrap();
		static void mathWrap();
		static void nodeWrap();
        static void constsWrap();
		//static void mathWrap( xxbind::facade_script * _facade, xxbind::module * _module );
		//static void nodeWrap( xxbind::facade_script * _facade, xxbind::module * _module );
		static void soundWrap();
		//static void soundWrap( xxbind::facade_script * _facade, xxbind::module * _module );
	};
}
