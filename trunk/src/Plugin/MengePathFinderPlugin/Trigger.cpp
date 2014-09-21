#	include "Trigger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Trigger::Trigger()
		: m_aoi(nullptr)
		, m_actor(nullptr)
		, m_data(nullptr)
		, m_radius(0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Trigger::~Trigger()
	{
		pybind::decref( m_data );
	}
	//////////////////////////////////////////////////////////////////////////
	void Trigger::setRadius( float _radius )
	{
		m_radius = _radius;
	}
	//////////////////////////////////////////////////////////////////////////
	float Trigger::getRadius() const
	{
		return m_radius;
	}
	//////////////////////////////////////////////////////////////////////////
	void Trigger::setAOI( AreaOfInterest * _aoi )
	{
		m_aoi = _aoi;
	}
	//////////////////////////////////////////////////////////////////////////
	AreaOfInterest * Trigger::getAOI() const
	{
		return m_aoi;
	}
	//////////////////////////////////////////////////////////////////////////
	void Trigger::setUserData( PyObject * _data )
	{
		pybind::decref( m_data );
		m_data = _data;
		pybind::incref( m_data );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Trigger::getUserData()
	{
		pybind::incref( m_data );

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
	void Trigger::onAOIActorEnter( AOIActor * _actor )
	{
		void * userData = _actor->getUserData();

		Trigger * trigger = static_cast<Trigger *>(userData);

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_TRIGGER_ENTER)( "(OO)", this->getEmbed(), trigger->getEmbed() );
	}
	//////////////////////////////////////////////////////////////////////////
	void Trigger::onAOIActorLeave( AOIActor * _actor )
	{
		void * userData = _actor->getUserData();

		Trigger * trigger = static_cast<Trigger *>(userData);

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_TRIGGER_LEAVE)( "(OO)", this->getEmbed(), trigger->getEmbed() );
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
	}
	//////////////////////////////////////////////////////////////////////////
	void Trigger::_setEventListener( PyObject * _listener )
	{
		Node::_setEventListener( _listener );

		this->registerEvent( EVENT_TRIGGER_ENTER, ("onTriggerEnter"), _listener );
		this->registerEvent( EVENT_TRIGGER_LEAVE, ("onTriggerLeave"), _listener );
	}
}