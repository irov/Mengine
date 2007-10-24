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
		void moveTo( size_t _time, const mt::vec2f & _point, bool _changeDirection );
		void moveStop();

	protected:
		bool _activate() override;
		void _update( size_t _timing, const Viewport & _viewport ) override;

	protected:
		bool m_moveTo;
		mt::vec2f m_movePoint;
		mt::vec2f m_moveDir;
		size_t m_moveTime;
		float m_moveSpeed;
	};
}