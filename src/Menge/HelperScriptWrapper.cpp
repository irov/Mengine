#	include "ScriptWrapper.h"

#	include "ScriptClassWrapperDefine.h"
#	include "ScriptDeclarationDefine.h"

#	include "Math/vec2.h"
#	include "Math/rand.h"

#	include "Color.h"

#	include <time.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class ScriptHelper
	{
	public:
		static int mt_randint( int a, int b )
		{			
			int big_a = a << 3;
			int big_b = b << 3;

			if( big_a == big_b )
			{
				return a;
			}

			int big_r = big_a  + ( rand() % (big_b - big_a) );
			big_r >>= 3;
			return big_r;
		}

		static float mt_sqrtf( float a )
		{
			return sqrtf(a);
		}

		static float mt_absf( float a )
		{
			return fabsf(a);
		}

		static float mt_cosf( float a )
		{
			return cosf(a);
		}

		static float mt_sinf( float a )
		{
			return sinf(a);
		}

		static float mt_tanf( float a )
		{
			return tanf( a );
		}

		static float mt_acosf( float a )
		{
			return acosf( a );
		}

		static float getVec2fX( const mt::vec2f & _vec2f )
		{
			return _vec2f.x;
		}

		static float getVec2fY( const mt::vec2f & _vec2f )
		{
			return _vec2f.y;
		}

		static float getVec3fX( const mt::vec3f & _vec3f )
		{
			return _vec3f.x;
		}

		static float getVec3fY( const mt::vec3f & _vec3f )
		{
			return _vec3f.y;
		}

		static float getVec3fZ( const mt::vec3f & _vec3f )
		{
			return _vec3f.z;
		}

		static float getA( const Color & _color )
		{
			return _color.a;
		}

		static float getR( const Color & _color )
		{
			return _color.r;
		}

		static float getG( const Color & _color )
		{
			return _color.g;
		}

		static float getB( const Color & _color )
		{
			return _color.b;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	//REGISTER_SCRIPT_CLASS( Menge, ScriptHelper, Base )
	void ScriptWrapper::helperWrap()
	{
		srand( (unsigned)time( NULL ) );

		pybind::def( "randint", &ScriptHelper::mt_randint );
		pybind::def( "sqrtf", &ScriptHelper::mt_sqrtf );
		pybind::def( "absf", &ScriptHelper::mt_absf );
		pybind::def( "cosf", &ScriptHelper::mt_cosf );
		pybind::def( "sinf", &ScriptHelper::mt_sinf );
		pybind::def( "tanf", &ScriptHelper::mt_tanf );
		pybind::def( "acosf", &ScriptHelper::mt_acosf );

		pybind::def( "getVec2fX", &ScriptHelper::getVec2fX );
		pybind::def( "getVec2fY", &ScriptHelper::getVec2fY );


		pybind::def( "getVec3fX", &ScriptHelper::getVec3fX );
		pybind::def( "getVec3fY", &ScriptHelper::getVec3fY );
		pybind::def( "getVec3fZ", &ScriptHelper::getVec3fZ );

		pybind::def( "getA", &ScriptHelper::getA );
		pybind::def( "getR", &ScriptHelper::getR );
		pybind::def( "getG", &ScriptHelper::getG );
		pybind::def( "getB", &ScriptHelper::getB );
	}
}