#	pragma once

#   include "Interface/EventInterface.h"

#	include "Kernel/EventEnum.h"

#	include "pybind/pybind.hpp"

#	include <vector>

namespace Menge
{
	class EventManager
        : public EventServiceInterface
	{
	public:
		EventManager();
		~EventManager();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		void addEventFormat( EEventName _event, PyObject * _method, const char * _format, ... ) override;
		void addEvent( EEventName _event, PyObject * _method, PyObject * _args ) override;

    public:
		void update() override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		struct Event
		{
			EEventName event;
			PyObject * method;
			PyObject * args;
		};

		typedef std::vector<Event> TVectorEvents;
		TVectorEvents m_proccessEvents;
		TVectorEvents m_addEvents;
	};
}