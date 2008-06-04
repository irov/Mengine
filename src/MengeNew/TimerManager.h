#	pragma once

#	include <list>

namespace Menge
{
	class TimerManager
	{
	public:
		TimerManager();

		struct TimerEvent
		{
			bool dead;
			bool updating;
			size_t id;
			float timing;
			PyObject * script;
		};

	public:
		size_t schedule( float _timing, PyObject * _func );
		void remove( size_t _id );
		void removeAll();

	public:
		void update( float _timing );
		void setUpdatable( bool _upatable );

	private:
		typedef std::list<ScheduleEvent> TListSchedules;

		bool m_updating;
		bool m_updatable;
		size_t m_schedulesID;
		TListSchedules m_schedules;
	};
}