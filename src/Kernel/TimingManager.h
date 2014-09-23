#	pragma once

#   include "Interface/TimingManagerInterface.h"

#	include <stdex/stl_list.h>

#	include "pybind/types.hpp"

namespace Menge
{
	struct TimingEventDesc
	{
		TimingListenerInterface * listener;

		float timing;
		float delay;

		uint32_t id;

		bool dead;
		bool freeze;

		bool portions;
		bool global;
	};

	class TimingManager
        : public TimingManagerInterface
	{
	public:
		TimingManager();
		~TimingManager();

    public:
        void initialize( ServiceProviderInterface * _serviceProvider ) override;

	public:
		uint32_t timing( bool _portions, bool _global, float _delay, TimingListenerInterface * _listener ) override;

    public:
		bool remove( uint32_t _timingID ) override;
		void removeAll( bool _global ) override;

		void freeze( uint32_t _id, bool _freeze ) override;
		void freezeAll( bool _freeze ) override;

		bool isFreeze( uint32_t _id ) const override;

	public:
		void update( float _current, float _timing ) override;

	private:
		bool findTimigEvent_( uint32_t _id, const TimingEventDesc *& _desc ) const;
		bool findTimigEvent_( uint32_t _id, TimingEventDesc *& _desc );

	protected:
		void destroyTiming_( TimingEventDesc & _desc );

	private:
        ServiceProviderInterface * m_serviceProvider;

		typedef stdex::list<TimingEventDesc> TListTimings;
		TListTimings m_timings;
		
		uint32_t m_enumerator;
		bool m_freeze;		
	};
}
