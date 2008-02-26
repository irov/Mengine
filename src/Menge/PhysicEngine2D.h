#	pragma once

#	include "Holder.h"
#	include "Math/vec2.h"
#	include "Math/polygon.h"

class PhysicSystem2DInterface;
class PhysicBody2DInterface;

namespace Menge
{

	class PhysicEngine2D
	{
	public:
		PhysicEngine2D(  PhysicSystem2DInterface * _interface );
		~PhysicEngine2D();

		void createScene( const mt::vec2f& _upperLeft, const mt::vec2f& _lowerRight, const mt::vec2f& _gravity, bool _doSleep = true );
		void destroyScene();

			
		PhysicBody2DInterface* createPhysicBody( const mt::polygon& _shape, const mt::vec2f& _pos, float _density, float _friction, float _restitution );
		void destroyPhysicBody( PhysicBody2DInterface* _bodyInterface );

		void update( float _timing, int _iterations );

	private:
		PhysicSystem2DInterface* m_interface;

	};
}	// namespace Menge