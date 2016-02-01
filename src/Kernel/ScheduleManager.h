#	pragma once

#   include "Interface/ScheduleManagerInterface.h"

#	include "pybind/types.hpp"

#	include <stdex/stl_vector.h>

namespace Menge
{    
	//////////////////////////////////////////////////////////////////////////
	enum ESchduleType
	{
		EST_EVENT,
		EST_TIMER
	};
	//////////////////////////////////////////////////////////////////////////
	struct ScheduleEventDesc
	{
		uint32_t id;

		ScheduleEventInterfacePtr event;
		ScheduleTimerInterfacePtr timer;
		SchedulePipeInterfacePtr pipe;

		float delay;
		float timing_delay;
		
		uint32_t iterate;

		ESchduleType type;
		bool dead;
		bool freeze;
		bool iterate_invalide;
	};
	//////////////////////////////////////////////////////////////////////////
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
		uint32_t event( float _delay, const ScheduleEventInterfacePtr & _listener ) override;

	public:
		uint32_t timing( const SchedulePipeInterfacePtr & _pipe, const ScheduleTimerInterfacePtr & _listener ) override;
		
	public:
		bool refresh( uint32_t _id ) override;

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
		float left( uint32_t _id ) const override;

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
		TListSchedules m_schedulesAdd;

		uint32_t m_enumerator;

		bool m_freezeAll;
		bool m_update;

    protected:
		bool findScheduleEvent_( uint32_t _id, const ScheduleEventDesc *& _desc ) const;
		bool findScheduleEvent_( uint32_t _id, ScheduleEventDesc *& _desc );
	};
}
