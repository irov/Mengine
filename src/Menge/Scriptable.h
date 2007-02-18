#	pragma once

namespace Menge
{
	class Scriptable
	{
		typedef Scriptable * (*FScriptRegistration)();

	public:
		Scriptable();
		virtual ~Scriptable();

	public:
		struct AutoRegistrator
		{
			AutoRegistrator( FScriptRegistration _func);
		};
	};
}

#	define MENGE_SCRIPTABLE_DEFINE()\
	public:\
	static Scriptable * scriptable_gen();\
	private:

#	define MENGE_SCRIPTABLE_IMPLEMENT( CLASS )\
	namespace Menge{\
	Scriptable * CLASS::scriptable_gen(){ return new CLASS(); }\
	static Scriptable::AutoRegistrator MD_auto_reg_##CLASS( &CLASS::scriptable_gen );\
}