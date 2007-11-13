#	pragma once

#	include "SceneNode2D.h"

namespace Menge
{
	class Scene;

	class Entity
		: public SceneNode2D
	{
		OBJECT_DECLARE( Entity )

	public:
		Entity();
		~Entity();

	public:
		void moveTo( float _time, const mt::vec2f & _point, bool _changeDirection );
		void rotateTo( float _time, const mt::vec2f & _point );
		void moveStop();
		void rotateStop();

	protected:
		bool _activate() override;
		void _deactivate() override;
		bool _compile() override;
		void _release() override;
		void _update( float _timing ) override;

	protected:
		bool m_moveTo;
		mt::vec2f m_movePoint;
		mt::vec2f m_moveDir;
		float m_moveTime;
		float m_moveSpeed;

		mt::vec2f m_targetDir;
		float m_rotateTime;
		bool m_rotate;
	};
}