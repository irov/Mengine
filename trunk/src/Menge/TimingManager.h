#	pragma once

#   include "Interface/TimingManagerInterface.h"

#	include <vector>

#	include "pybind/types.hpp"

namespace Menge
{
    class ServiceProviderInterface;

	class TimingManager
        : public TimingManagerInterface
	{
	public:
		TimingManager();
		~TimingManager();

    public:
        void initialize( ServiceProviderInterface * _serviceProvider ) override;

    public:
		struct TimingEvent
		{
			TimingListener * listener;

			float timing;
			float delay;

			size_t id;

			bool remove;
			bool dead;
			bool freeze;

			bool portions;
			bool global;
		};

	public:
		size_t timing( bool _portions, bool _global, float _delay, TimingListener * _listener ) override;

    public:
		void remove( size_t _timingID ) override;
		void removeAll( bool _global ) override;

		void freeze( size_t _id, bool _freeze ) override;
		void freezeAll( bool _freeze ) override;

		bool isFreeze( size_t _id ) const override;

	public:
		void update( float _current, float _timing ) override;

	private:
		bool findTimigEvent_( size_t _id, const TimingEvent *& _event ) const;
		bool findTimigEvent_( size_t _id, TimingEvent *& _event );

	private:
        ServiceProviderInterface * m_serviceProvider;

		typedef std::vector<TimingEvent> TListTimings;
		TListTimings m_events;
		
		size_t m_enumerator;
		bool m_freeze;		
	};
}
