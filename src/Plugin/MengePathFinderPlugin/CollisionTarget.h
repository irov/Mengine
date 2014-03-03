#	pragma once

#	include "Math/vec3.h"

namespace Menge
{
	class CollisionObject;

	class CollisionTarget
	{
	public:
		virtual const mt::vec3f & getPosition() const = 0;
	};

	class CollisionTargetPosition
		: public CollisionTarget
	{
	public:
		CollisionTargetPosition();

	public:
		void setPosition( const mt::vec3f & _position );

	protected:
		const mt::vec3f & getPosition() const override;

	protected:
		mt::vec3f m_position;
	};

	class CollisionTargetFollow
		: public CollisionTarget
	{
	public:
		CollisionTargetFollow();

	public:
		void setFollowObject( CollisionObject * _object );
		void setDistance( float _distance );

	protected:
		const mt::vec3f & getPosition() const override;

	protected:
		CollisionObject * m_object;
		float m_distance;
	};
}