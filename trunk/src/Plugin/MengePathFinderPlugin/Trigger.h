#	pragma once

#	include "Kernel/Node.h"

#	include "AreaOfInterest.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	class Trigger
		: public Node
		, public AOIActorProviderInterface
	{
	public:
		Trigger();
		~Trigger();

	public:
		void setTriggerRadius( float _radius );
		float getTriggerRadius() const;
		
	public:
		void setTriggerIFF( uint32_t _iff );
		uint32_t getTriggerIFF() const;

	public:
		void setTriggerAOI( AreaOfInterest * _aoi );
		AreaOfInterest * getTriggerAOI() const;

	public:
		void setTriggerUserData( PyObject * _data );
		PyObject * getTriggerUserData();

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
		void _setEventListener( const pybind::dict & _listener ) override;

	protected:
		AreaOfInterest * m_aoi;
		AOIActor * m_actor;

		PyObject * m_data;
		
		float m_radius;
		
		uint32_t m_iff;
	};
}