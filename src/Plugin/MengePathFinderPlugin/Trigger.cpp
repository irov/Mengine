#	include "Trigger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Trigger::Trigger()
		: m_aoi(nullptr)
		, m_actor(nullptr)
		, m_radius(0.f)
		, m_iff(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Trigger::~Trigger()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Trigger::setTriggerRadius( float _radius )
	{
		m_radius = _radius;
	}
	//////////////////////////////////////////////////////////////////////////
	float Trigger::getTriggerRadius() const
	{
		return m_radius;
	}
	//////////////////////////////////////////////////////////////////////////
	void Trigger::setTriggerIFF( uint32_t _iff )
	{
		m_iff = _iff;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t Trigger::getTriggerIFF() const
	{
		return m_iff;
	}
	//////////////////////////////////////////////////////////////////////////
	void Trigger::setTriggerAOI( AreaOfInterest * _aoi )
	{
		m_aoi = _aoi;
	}
	//////////////////////////////////////////////////////////////////////////
	AreaOfInterest * Trigger::getTriggerAOI() const
	{
		return m_aoi;
	}
	//////////////////////////////////////////////////////////////////////////
	void Trigger::setTriggerUserData( const pybind::object & _data )
	{
		m_data = _data;
	}
	//////////////////////////////////////////////////////////////////////////
	const pybind::object & Trigger::getTriggerUserData() const
	{
		return m_data;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f Trigger::getAOIActorPosition() const
	{
		const mt::vec3f & pos = this->getWorldPosition();

		return mt::vec2f(pos.x, pos.y);
	}
	//////////////////////////////////////////////////////////////////////////
	float Trigger::getAOIActorRadius() const
	{
		return m_radius;
	}
	//////////////////////////////////////////////////////////////////////////
	enum TriggerEventFlag
	{
		EVENT_TRIGGER_ENTER = 0,
		EVENT_TRIGGER_LEAVE
	};
	//////////////////////////////////////////////////////////////////////////
	void Trigger::_setEventListener( const pybind::dict & _listener )
	{
		Node::_setEventListener( _listener );

		this->registerEvent( EVENT_TRIGGER_ENTER, ("onTriggerEnter"), _listener );
		this->registerEvent( EVENT_TRIGGER_LEAVE, ("onTriggerLeave"), _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void Trigger::onAOIActorEnter( AOIActor * _actor )
	{
		void * userData = _actor->getUserData();

		Trigger * trigger = static_cast<Trigger *>(userData);

		uint32_t enemu_iff = trigger->getTriggerIFF();

		EVENTABLE_CALL( m_serviceProvider, this, EVENT_TRIGGER_ENTER )(this, trigger, m_iff, enemu_iff);
	}
	//////////////////////////////////////////////////////////////////////////
	void Trigger::onAOIActorLeave( AOIActor * _actor )
	{
		void * userData = _actor->getUserData();

		Trigger * trigger = static_cast<Trigger *>(userData);

		uint32_t enemu_iff = trigger->getTriggerIFF();

		EVENTABLE_CALL( m_serviceProvider, this, EVENT_TRIGGER_LEAVE )(this, trigger, m_iff, enemu_iff);
	}
	//////////////////////////////////////////////////////////////////////////
	bool Trigger::_activate()
	{
		if( m_aoi == nullptr )
		{
			return false;
		}

		m_actor = m_aoi->createActor( this, this );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Trigger::_deactivate()
	{
		m_aoi->removeActor( m_actor );
		m_actor = nullptr;
	}
}