#	pragma once

#	include "Kernel/Node.h"

#	include "AreaOfInterest.h"

namespace Menge
{
	class NodeAOITrigger
		: public Node
		, public Eventable
		, public AOITriggerProviderInterface
	{
	public:
		NodeAOITrigger();
		~NodeAOITrigger();

	public:
		void setRadius( float _radius );
		float getRadius() const;
				
	public:
		void setIFF( uint32_t _iff );
		uint32_t getIFF() const;

	public:
		void setAOI( AreaOfInterest * _aoi );
		AreaOfInterest * getAOI() const;

	public:
		void setTriggerUserData( const pybind::object & _data );
		const pybind::object & getTriggerUserData() const;

	protected:
		bool onAOIActorTest( AOIActorProviderInterface * _actor ) const override;
		void onAOIActorEnter( AOIActorProviderInterface * _actor ) override;
		void onAOIActorLeave( AOIActorProviderInterface * _actor ) override;

	protected:
		bool _activate() override;
		void _deactivate() override;

	protected:
		void _setEventListener( const pybind::dict & _listener ) override;

	protected:
		AreaOfInterest * m_aoi;
		AOITriggerPtr m_trigger;

		pybind::object m_data;
		
		float m_radius;
		
		uint32_t m_iff;
	};
}