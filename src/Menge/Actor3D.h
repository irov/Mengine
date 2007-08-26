#	pragma once

#	include "SceneNode3D.h"

#	include "AnimationObject.h"

namespace Menge
{
	class Actor3D
		: public SceneNode3D
	{
		OBJECT_DECLARE(Actor3D)

	public:
		Actor3D();

	public:
		void	moveTo(const mt::vec3f& _wayPoint);
		void	lookTo(const mt::vec3f& _target);
		void	stop();

		bool	isMoving() const;

	public:
		void	loader( TiXmlElement * _xml ) override;
		
	protected:
		void	_update( float _timing)  override;
		bool	_activate() override;
		void	_deactivate() override;
		void	_render( const mt::mat4f & _rwm, const Camera3D * _camera ) override;

	protected:
		AnimationObject * m_animObject;

	private:

		enum eMovementState
		{
			STOP,
			MOVE,
			ROTATE,
		};

		eMovementState m_state;

		float		m_speed;
		float		m_maxSpeed;
		float		m_rotateSpeed;
		float		m_acceleration;

		mt::vec3f   m_destPos;
		mt::vec3f   m_destDir;
		bool		m_lookAtTarget;

		float		_calculateNewSpeed( float _timing );
		void		_calculateNewPosition( float _timing );
		void		_calculateNewDirection( float _timing );
		mt::vec3f &	_calculateDirection();
		float		_calculateDistance();
	};	
}