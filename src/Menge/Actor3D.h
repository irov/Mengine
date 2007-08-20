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
		void	moveToWayPoint(const mt::vec3f& _wayPoint);
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
		bool		m_isMove;
		mt::vec3f	m_dir;
		mt::vec3f   m_destpos;
		
		float		m_acceleration;
		float		m_speed;
		float		m_maxSpeed;
	};	
}