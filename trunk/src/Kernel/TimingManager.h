#	pragma once

#   include "Interface/TimingManagerInterface.h"

#	include <stdex/stl_list.h>

#	include "pybind/types.hpp"

namespace Menge
{
	struct TimingEventDesc
	{
		TimingListenerInterfacePtr listener;

		float timing;
		float delay;

		uint32_t id;

		bool dead;
		bool freeze;
	};

	class TimingManager
        : public TimingManagerInterface
	{
	public:
		TimingManager();
		~TimingManager();

    public:        
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;
		
	public:
		uint32_t timing( float _delay, const TimingListenerInterfacePtr & _listener ) override;

    public:
		bool remove( uint32_t _timingID ) override;
		void removeAll() override;

		void freeze( uint32_t _id, bool _freeze ) override;
		void freezeAll( bool _freeze ) override;

		bool isFreeze( uint32_t _id ) const override;

	public:
		void setSpeedFactor( float _factor ) override;
		float getSpeedFactor() const override;

	public:
		void update( float _current, float _timing ) override;

	private:
		bool findTimigEvent_( uint32_t _id, const TimingEventDesc *& _desc ) const;
		bool findTimigEvent_( uint32_t _id, TimingEventDesc *& _desc );

	protected:
		void destroyTiming_( TimingEventDesc & _desc );

	private:
        ServiceProviderInterface * m_serviceProvider;

		float m_speedFactor;

		typedef stdex::list<TimingEventDesc> TListTimings;
		TListTimings m_timings;
		
		uint32_t m_enumerator;
		bool m_freeze;		
	};
}
