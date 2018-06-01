#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/Scriptable.h"
#include "Kernel/Node.h"

#include "Factory/Factorable.h"

#include "Core/ConstString.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class BounceActorTransformationProvider
	{
	public:
		virtual const mt::vec3f & getBounceActorPosition() const = 0;
		virtual void translateBounceActor( const mt::vec3f & _value ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	class BounceActor
		: public Factorable
		, public Scriptable
	{
	public:
		BounceActor();
		~BounceActor() override;
        
	public:
		void setRadius( float _radius );
		float getRadius() const;

		void setMass( float _mass );
		float getMass() const;

		void setNode( Node * _node );
		Node * getNode() const;

	public:
		void remove();
		bool isRemoved() const;

	protected:
		float m_radius;
		float m_mass;

		Node * m_node;

		bool m_remove;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<class BounceActor> BounceActorPtr;
}