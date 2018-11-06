#pragma once

#include "Box2D/Box2D.h"

namespace Mengine
{
	inline static float32 Box2DScalerToWorld( float _v )
	{
		return (float32)_v * 0.01f;
	}

	inline static float Box2DScalerFromWorld( float32 _v )
	{
		return (float32)_v * 100.f;
	}

	inline static b2Vec2 Box2DScalerToWorld( const mt::vec2f & _v )
	{
		return b2Vec2( Box2DScalerToWorld( _v.x ), Box2DScalerToWorld( _v.y ) );
	}

	inline static mt::vec2f Box2DScalerFromWorld( const b2Vec2 & _v )
	{
		return mt::vec2f( Box2DScalerFromWorld( _v.x ), Box2DScalerFromWorld( _v.y ) );
	}
}