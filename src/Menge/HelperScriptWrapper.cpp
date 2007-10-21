#	include "ScriptClassWrapperDefine.h"
#	include "ScriptDeclarationDefine.h"

#	include "Math/vec2.h"
#	include "Math/rand.h"

#	include <time.h>

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

		static float getVec2fX( const mt::vec2f & _vec2f )
		{
			return _vec2f.x;
		}

		static float getVec2fY( const mt::vec2f & _vec2f )
		{
			return _vec2f.y;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	REGISTER_SCRIPT_CLASS( Menge, ScriptHelper, Base )
	{
		srand( clock() );

		pybind::def( "randint", &ScriptHelper::randint );

		pybind::def( "getVec2fX", &ScriptHelper::getVec2fX );
		pybind::def( "getVec2fY", &ScriptHelper::getVec2fY );
	}
}