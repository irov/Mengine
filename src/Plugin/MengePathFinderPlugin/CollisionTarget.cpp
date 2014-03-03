#	include "CollisionTarget.h"
#	include "CollisionObject.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	CollisionTargetPosition::CollisionTargetPosition()
		: m_position(0.f, 0.f, 0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionTargetPosition::setPosition( const mt::vec3f & _position )
	{
		m_position = _position;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & CollisionTargetPosition::getPosition() const
	{
		return m_position;
	}
	//////////////////////////////////////////////////////////////////////////
	CollisionTargetFollow::CollisionTargetFollow()
		: m_object(nullptr)
		, m_distance(0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionTargetFollow::setFollowObject( CollisionObject * _object )
	{
		m_object = _object;
	}
	//////////////////////////////////////////////////////////////////////////
	void CollisionTargetFollow::setDistance( float _distance )
	{
		m_distance = _distance;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & CollisionTargetFollow::getPosition() const
	{
		const mt::vec3f & wp = m_object->getWorldPosition();

		return wp;
	}

}
