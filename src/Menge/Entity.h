#	pragma once

#	include "RigidBody2D.h"
#	include "ValueInterpolator.h"

namespace Menge
{
	class Scene;

	class Entity
		: public RigidBody2D
	{
		OBJECT_DECLARE( Entity )

	public:
		Entity();
		~Entity();

	public:
		void moveTo( float _time, const mt::vec2f & _point, bool _changeDirection );
		void rotateTo( float _time, const mt::vec2f & _point );
		void accelerateTo( float _speed, const mt::vec2f& _point, bool _changeDirection );
		void directTo( float _time, const mt::vec2f& _dir );
		void angleTo( float _time, float _angle );
		void moveStop();
		void rotateStop();
		void flip( bool _x, bool _y );
		
		//void setScale( const mt::vec2f& _scale );
		//const mt::vec2f& getScale() const;

		void setVelocity( const mt::vec2f& _velocity );
		const mt::vec2f& getVelocity() const;
		float getVelocitySpeed() const;

		void scaleTo( float _time, const mt::vec2f& _scale );
		void scaleStop();
		
		void loader( XmlElement * _xml );
		void onCollide( PhysicBody2DInterface* _otherObj, float _worldX, float _worldY, float _normalX, float _normalY );

	protected:
		bool _activate() override;
		void _deactivate() override;
		bool _compile() override;
		void _release() override;
		void _update( float _timing ) override;
		void _render( unsigned int _debugMask ) override;

	protected:
		mt::vec2f m_velocity;
		mt::vec2f m_acceleration;
		//float m_acceleration;
		float m_acTime;

		bool m_moveTo;
		mt::vec2f m_movePoint;
		float m_moveTime;

		//mt::vec2f m_scale;
		ValueInterpolatorLinear<mt::vec2f> m_scaleTo;

		/*mt::vec2f m_targetDir;
		float m_rotateTime;
		bool m_rotate;*/
		float m_angle;
		ValueInterpolatorLinear<float> m_rotateTo;

		bool m_accelerateTo;
		mt::vec2f m_nSpeed;

		bool m_physicController;


		bool m_onUpdateEvent;
	};
}
