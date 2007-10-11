#	pragma once

#	include <list>

extern "C" 
{ 
	struct _object; 
	typedef _object PyObject;
}

namespace Menge
{
	struct ScheduleEvent;

	class ScheduleManager
	{
	public:
		ScheduleManager();

	public:
		size_t schedule( size_t _timing, PyObject * _func );
		void remove( size_t _id );

	public:
		void update( size_t _timing );

	private:
		typedef std::list<ScheduleEvent *> TSchedules;

		size_t m_schedulesID;
		TSchedules m_schedules;
	};
}