#	pragma once

#	include "EventEnum.h"

#	include <Core/Holder.h>

#	include "pybind/pybind.hpp"

#	include <vector>

namespace Menge
{
	class EventManager
		: public Holder<EventManager>
	{
	public:
		EventManager();
		~EventManager();

	public:
		void addEvent( EEventName _event, PyObject * _method, PyObject * _args );
		void update();


	protected:
		struct Event
		{
			EEventName event;
			PyObject * method;
			PyObject * args;
		};

		typedef std::vector<Event> TVectorEvents;
		TVectorEvents m_events;
	};
}