#	include "ScriptClassWrapperDefine.h"
#	include "ScriptDeclarationDefine.h"

#include <stdlib.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class ScriptHelper
	{
	public:
		static int randint( int a, int b )
		{
			return int(a + ( rand() % (b - a) ) );
		}
	};
	//////////////////////////////////////////////////////////////////////////
	REGISTER_SCRIPT_CLASS( Menge, ScriptHelper, Base )
	{
		pybind::def( "randint", &ScriptHelper::randint );
	}
}