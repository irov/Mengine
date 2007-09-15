#	pragma once

#	include "SceneNode3D.h"

#	include "Animation.h"

namespace Menge
{
	class Animation;

	class Actor2D
		: public SceneNode2D
	{
		OBJECT_DECLARE(Actor2D)

	public:
		Actor2D();

	public:
		void	moveTo( const mt::vec2f& _target );
		void	lookTo( const mt::vec2f& _target );
		void	stop();

		bool	isMoving() const;

	public:
		void	loader( TiXmlElement * _xml ) override;
		
	protected:
		void	_update( float _timing )  override;
		bool	_activate() override;
		void	_deactivate() override;
		void	_render( const mt::mat3f & _rwm, const Viewport & _viewport ) override;

	protected:
		Animation * m_animObject;

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

		mt::vec2f   m_destPos;
		mt::vec2f   m_destDir;
		bool		m_lookAtTarget;

		float		_calculateNewSpeed( float _timing );
		void		_calculateNewPosition( float _timing );
		void		_calculateNewDirection( float _timing );
		mt::vec2f &	_calculateDirection();
		float		_calculateDistance();
	};	
}