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
			size_t id;

			bool dead;
			bool freeze;
			bool skip;
		};

	public:
		size_t schedule( float _timing, PyObject * _func );

		void skip( size_t _id );

		void remove( size_t _id );
		void removeAll();

		void freeze( size_t _id, bool _freeze );
		void freezeAll( bool _freeze );
		bool isFreeze( size_t _id ) const;

		float time( size_t _id ) const;

	public:
		void update( float _timing );

	private:
		void callEvent_( const ScheduleEvent & _event, bool _isEnd );

	private:
		typedef std::vector<ScheduleEvent> TListSchedules;
		TListSchedules m_schedules;

		size_t m_enumerator;	
		bool m_freeze;
	};
}
