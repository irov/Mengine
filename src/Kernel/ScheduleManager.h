#	pragma once

#   include "Interface/ScheduleManagerInterface.h"

#	include "pybind/types.hpp"

#	include <vector>

namespace Menge
{
    class ServiceProviderInterface;
    
	class ScheduleManager
        : public ScheduleManagerInterface
	{
	public:
		ScheduleManager();
		~ScheduleManager();

    public:
        void initialize( ServiceProviderInterface * _serviceProvider ) override;

    public:
		struct ScheduleEvent
		{
			ScheduleListenerPtr listener;

			float timing;
			size_t id;

			mutable bool dead;
			mutable bool freeze;
		};

	public:
		size_t schedule( float _timing, const ScheduleListenerPtr & _listener ) override;

    public:
		bool remove( size_t _id ) override;
		void removeAll() override;

		bool freeze( size_t _id, bool _freeze ) override;
		void freezeAll( bool _freeze ) override;
		bool isFreezeAll() const override;
		bool isFreeze( size_t _id ) const override;

		float time( size_t _id ) const override;
		
	public:
		void update( float _current, float _timing ) override;

    protected:
        bool removeEvent_( const ScheduleEvent & _event );

	protected:
        ServiceProviderInterface * m_serviceProvider;

		typedef std::vector<ScheduleEvent> TListSchedules;
		TListSchedules m_schedules;

		size_t m_enumeratorSchedule;
		bool m_freezeAll;

    protected:
        const ScheduleEvent * findEvent_( size_t _id ) const;
	};
}
