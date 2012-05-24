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
		virtual bool update( size_t _id, float _timing ) = 0;
		virtual void remove( size_t _id ) = 0;
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

			size_t id;

			bool remove;
			bool dead;
			bool freeze;

			bool portions;
			bool global;
		};

	public:
		size_t timing( bool _portions, bool _global, float _delay, TimingListener * _listener );

		void remove( size_t _timingID );
		void removeAll( bool _global );

		void freeze( size_t _id, bool _freeze );
		void freezeAll( bool _freeze );

		bool isFreeze( size_t _id ) const;

	public:
		void update( float _timing );

	private:
		bool findTimigEvent_( size_t _id, const TimingEvent *& _event ) const;
		bool findTimigEvent_( size_t _id, TimingEvent *& _event );

	private:
		typedef std::vector<TimingEvent> TListTimings;
		TListTimings m_events;
		
		size_t m_enumerator;
		bool m_freeze;		
	};
}
