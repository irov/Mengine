#	pragma once

#	include "SceneNode2D.h"

namespace Menge
{
	class Actor2D
		: public SceneNode2D
	{
		OBJECT_DECLARE(Actor2D)

	public:
		Actor2D();

	public:
		void	moveToWayPoint(const mt::vec2f& _wayPoint);
		void	stop();
		bool	isMoving() const;

	protected:
		void	loader( TiXmlElement * _xml ) override;
		void	_update( float _timing)  override;

	private:
		bool		m_isMove;
		mt::vec2f	m_dir;
		mt::vec2f   m_destpos;
		float		m_velocity;
	};	
}