#	pragma once

#	include <vector>

#	include "pybind/types.hpp"

namespace Menge
{
	class ScheduleManager
	{
	public:
		ScheduleManager();
		~ScheduleManager();

		struct ScheduleEvent
		{
			PyObject * script;
			float timing;
			std::size_t id;

			bool dead;
			bool freeze;
			bool skip;
		};

	public:
		std::size_t schedule( float _timing, PyObject * _func );

		void skip( std::size_t _id );

		void remove( std::size_t _id );
		void removeAll();

		void freeze( std::size_t _id, bool _freeze );
		void freezeAll( bool _freeze );
		bool isFreeze( std::size_t _id ) const;

		float time( std::size_t _id ) const;

	public:
		void update( float _timing );

	private:
		void callEvent_( const ScheduleEvent & _event, bool _isEnd );

	private:
		typedef std::vector<ScheduleEvent> TListSchedules;
		TListSchedules m_schedules;

		std::size_t m_enumerator;	
		bool m_freeze;
	};
}
