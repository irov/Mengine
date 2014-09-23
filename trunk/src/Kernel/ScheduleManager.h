#	pragma once

#   include "Interface/ScheduleManagerInterface.h"

#	include "pybind/types.hpp"

#	include <stdex/stl_vector.h>

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
			uint32_t id;

			mutable bool dead;
			mutable bool freeze;
		};

	public:
		uint32_t schedule( float _timing, const ScheduleListenerPtr & _listener ) override;

    public:
		bool remove( uint32_t _id ) override;
		void removeAll() override;

		bool freeze( uint32_t _id, bool _freeze ) override;
		void freezeAll( bool _freeze ) override;
		bool isFreezeAll() const override;
		bool isFreeze( uint32_t _id ) const override;

		float time( uint32_t _id ) const override;
		
	public:
		void update( float _current, float _timing ) override;

    protected:
        bool removeEvent_( const ScheduleEvent & _event );

	protected:
        ServiceProviderInterface * m_serviceProvider;

		typedef stdex::vector<ScheduleEvent> TListSchedules;
		TListSchedules m_schedules;

		uint32_t m_enumeratorSchedule;
		bool m_freezeAll;

    protected:
        const ScheduleEvent * findEvent_( uint32_t _id ) const;
	};
}
