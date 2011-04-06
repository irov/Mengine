#	pragma once

#	include <vector>

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
			PyObject * script;
			float timing;
			std::size_t id;

			bool dead;
			bool updating;
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
		typedef std::vector<ScheduleEvent> TListSchedules;

		bool m_updating;
		bool m_freeze;
		std::size_t m_enumerator;
		TListSchedules m_schedules;
		TListSchedules m_schedulesToAdd;

	private:
		void callEvent_( const ScheduleEvent & _event, bool _isEnd );
	};
}
