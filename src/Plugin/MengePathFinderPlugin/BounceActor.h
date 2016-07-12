#	pragma once

#	include "Interface/ServiceInterface.h"

#	include "Kernel/Scriptable.h"
#	include "Kernel/Node.h"

#	include "Factory/FactorablePtr.h"

#	include "Core/ConstString.h"

#   include "stdex/intrusive_ptr.h"

namespace Menge
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
		: public FactorablePtr
		, public Scriptable
	{
	public:
		BounceActor();
		~BounceActor();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider );
		ServiceProviderInterface * getServiceProvider() const;

		void setType( const ConstString & _type );
		const ConstString & getType() const;

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
		ServiceProviderInterface * m_serviceProvider;
		ConstString m_type;

		float m_radius;
		float m_mass;

		Node * m_node;

		bool m_remove;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<class BounceActor> BounceActorPtr;
}