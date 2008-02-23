#	pragma once

namespace Menge
{

	class PhysicEngine2D
	{
	public:
		PhysicEngine2D();
		~PhysicEngine2D();

		void createScene( const mt::vec2f& _upperLeft, const mt::vec2f& _lowerRight, const mt::vec2f& _gravity, bool _doSleep = true );
		void destroyScene();


	};
}	// namespace Menge