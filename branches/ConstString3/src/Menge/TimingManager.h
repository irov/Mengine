#	pragma once

#	include <vector>

#	include "pybind/types.hpp"

namespace Menge
{
	class Timing
	{
	public:
		virtual ~Timing(){};

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
			Timing * timing;

			std::size_t id;
			bool dead;
			bool freeze;
		};

	public:
		std::size_t add( Timing * _timing );
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
		TListTimings m_timings;

		std::size_t m_enumerator;
		bool m_freeze;		
	};
}
