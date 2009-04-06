#	pragma once

#	include <list>

extern "C" 
{ 
	struct _object; 
	typedef _object PyObject;
}

namespace Menge
{
	class ScheduleManager
	{
	public:
		ScheduleManager();
		~ScheduleManager();

		struct ScheduleEvent
		{
			bool dead;
			bool updating;
			bool paused;
			std::size_t id;
			float timing;
			PyObject * script;
		};

	public:
		std::size_t schedule( float _timing, PyObject * _func );
		std::size_t timerSchedule( float _timing, PyObject* _func );
		void remove( std::size_t _id );
		void removeAll();
		void freeze( std::size_t _id, bool _freeze );

	public:
		void update( float _timing );
		void setUpdatable( bool _upatable );

	private:
		typedef std::list<ScheduleEvent> TListSchedules;

		bool m_updating;
		bool m_updatable;
		std::size_t m_schedulesID;
		TListSchedules m_schedules;
		TListSchedules m_timerSchedules;
	};
}
