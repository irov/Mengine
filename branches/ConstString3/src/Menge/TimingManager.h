#	pragma once

#	include <vector>

#	include "pybind/types.hpp"

namespace Menge
{
	class TimingListener
	{
	public:
		virtual ~TimingListener(){};

	public:
		virtual bool update( std::size_t _id, float _timing ) = 0;
	};

	class TimingManager
	{
	public:
		TimingManager();
		~TimingManager();

		struct TimingEvent
		{
			TimingListener * listener;

			float timing;
			float delay;

			std::size_t id;
			bool dead;
			bool freeze;

			bool portions;
		};

	public:
		std::size_t timing( bool _portions, float _delay, TimingListener * _listener );

		void remove( std::size_t _timingID );
		void removeAll();

		void freeze( std::size_t _id, bool _freeze );
		void freezeAll( bool _freeze );
		bool isFreeze( std::size_t _id ) const;

	public:
		void update( float _timing );

	private:
		bool findTimigEvent_( std::size_t _id, const TimingEvent *& _event ) const;
		bool findTimigEvent_( std::size_t _id, TimingEvent *& _event );

	private:
		typedef std::vector<TimingEvent> TListTimings;
		TListTimings m_events;
		
		std::size_t m_enumerator;
		bool m_freeze;		
	};
}
