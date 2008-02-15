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
		void accelerateTo( float _speed, const mt::vec2f& _point, bool _changeDirection );
		void moveStop();
		void rotateStop();
		void flip( bool _x, bool _y );
		void setScale( const mt::vec2f& _scale );
		const mt::vec2f& getScale() const;
		const mt::vec2f& getSpeedVec() const;
		float getSpeed() const;
		void scaleTo( float _time, const mt::vec2f& _scale );
		void scaleStop();
		void setSpeed( const mt::vec2f& _speed );
		//void setAcceleration( const mt::vec2f& _acceleration )	{ m_acceleration = _acceleration; }

	protected:
		bool _activate() override;
		void _deactivate() override;
		bool _compile() override;
		void _release() override;
		void _update( float _timing ) override;

	protected:
		mt::vec2f m_speed;
		mt::vec2f m_acceleration;
		//float m_acceleration;
		float m_acTime;

		bool m_moveTo;
		mt::vec2f m_movePoint;
		float m_moveTime;

		bool m_scaleTo;
		mt::vec2f m_scalePoint;
		float m_scaleTime;

		mt::vec2f m_targetDir;
		float m_rotateTime;
		bool m_rotate;
		mt::vec2f m_scale;

		bool m_accelerateTo;
		mt::vec2f m_nSpeed;
	};
}