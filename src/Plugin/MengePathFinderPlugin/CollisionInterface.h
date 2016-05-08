#	pragma once

#	include "Factory/FactorablePtr.h"

#	include "Math/vec2.h"

#   include "stdex/intrusive_ptr.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class CollisionInterface
		: public FactorablePtr
	{
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<CollisionInterface> CollisionInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class CollisionActorProviderInterface
	{
	public:
		virtual void onCollisionPositionProvider( mt::vec2f & _position ) const = 0;
		virtual bool onCollisionTest( CollisionActorProviderInterface * _actor, const mt::vec2f & _point, const mt::vec2f & _normal, float _penetration ) = 0;
	};
}