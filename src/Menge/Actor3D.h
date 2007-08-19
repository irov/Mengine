#	pragma once

#	include "SceneNode3D.h"

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

	protected:
		void	loader( TiXmlElement * _xml ) override;
		void	_update( float _timing)  override;

	private:
		bool		m_isMove;
		mt::vec3f	m_dir;
		mt::vec3f   m_destpos;
		float		m_velocity;
	};	
}