#	pragma once

#   include "Interface/ScheduleManagerInterface.h"

#	include "pybind/types.hpp"

#	include <stdex/stl_vector.h>

namespace Menge
{    
	struct ScheduleEventDesc
	{
		ScheduleListenerInterfacePtr listener;

		float timing;
		float delay;
		uint32_t id;

		bool update;
		bool dead;
		bool freeze;
	};
    
	class ScheduleManager
        : public ScheduleManagerInterface
	{
	public:
		ScheduleManager();
		~ScheduleManager();

	public:        
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

	public:
		uint32_t schedule( float _timing, const ScheduleListenerInterfacePtr & _listener ) override;

	public:
		uint32_t timing( float _delay, const ScheduleListenerInterfacePtr & _listener ) override;

	public:
		bool refresh( uint32_t _id, float _timing ) override;

	public:
		bool exist( uint32_t _id ) const override;

    public:
		bool remove( uint32_t _id ) override;
		void removeAll() override;

		bool freeze( uint32_t _id, bool _freeze ) override;
		void freezeAll( bool _freeze ) override;
		bool isFreezeAll() const override;
		bool isFreeze( uint32_t _id ) const override;

		float time( uint32_t _id ) const override;

	public:
		void setSpeedFactor( float _factor ) override;
		float getSpeedFactor() const override;

	public:
		float getTiming() const override;
		
	public:
		void update( float _current, float _timing ) override;

    protected:
        bool removeSchedule_( ScheduleEventDesc & _event );

	protected:
        ServiceProviderInterface * m_serviceProvider;

		float m_speedFactor;
		float m_timing;

		typedef stdex::vector<ScheduleEventDesc> TListSchedules;
		TListSchedules m_schedules;

		uint32_t m_enumeratorSchedule;
		bool m_freezeAll;

    protected:
		bool findScheduleEvent_( uint32_t _id, const ScheduleEventDesc *& _desc ) const;
		bool findScheduleEvent_( uint32_t _id, ScheduleEventDesc *& _desc );
	};
}
