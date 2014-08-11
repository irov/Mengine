#	pragma once

#	include "Kernel/Node.h"

#	include "AreaOfInterest.h"

namespace Menge
{
	class Trigger
		: public Node
		, public AOIActorProviderInterface
	{
	public:
		Trigger();

	public:
		void setRadius( float _radius );
		float getRadius() const;

	public:
		void setAOI( AreaOfInterest * _aoi );
		AreaOfInterest * getAOI() const;

	protected:
		mt::vec2f getAOIActorPosition() const override;
		float getAOIActorRadius() const override;

	protected:
		void onAOIActorEnter( AOIActor * _actor ) override;
		void onAOIActorLeave( AOIActor * _actor ) override;

	protected:
		bool _activate() override;
		void _deactivate() override;

	protected:
		void _setEventListener( PyObject * _listener ) override;

	protected:
		AreaOfInterest * m_aoi;
		AOIActor * m_actor;
		
		float m_radius;
	};
}