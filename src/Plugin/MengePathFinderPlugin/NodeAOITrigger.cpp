#	include "NodeAOITrigger.h"
#	include "NodeAOIActor.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	NodeAOITrigger::NodeAOITrigger()
		: m_aoi(nullptr)
		, m_radius(0.f)
		, m_iff(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	NodeAOITrigger::~NodeAOITrigger()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeAOITrigger::setRadius( float _radius )
	{
		m_radius = _radius;
	}
	//////////////////////////////////////////////////////////////////////////
	float NodeAOITrigger::getRadius() const
	{
		return m_radius;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeAOITrigger::setIFF( uint32_t _iff )
	{
		m_iff = _iff;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t NodeAOITrigger::getIFF() const
	{
		return m_iff;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeAOITrigger::setAOI( AreaOfInterest * _aoi )
	{
		m_aoi = _aoi;
	}
	//////////////////////////////////////////////////////////////////////////
	AreaOfInterest * NodeAOITrigger::getAOI() const
	{
		return m_aoi;
	}
	//////////////////////////////////////////////////////////////////////////
	enum TriggerEventFlag
	{
		EVENT_TRIGGER_ENTER = 0,
		EVENT_TRIGGER_LEAVE
	};
	//////////////////////////////////////////////////////////////////////////
	void NodeAOITrigger::_setEventListener( const pybind::dict & _listener )
	{
		Node::_setEventListener( _listener );

		this->registerEvent( EVENT_TRIGGER_ENTER, ("onTriggerEnter"), _listener );
		this->registerEvent( EVENT_TRIGGER_LEAVE, ("onTriggerLeave"), _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeAOITrigger::onAOIActorTest( AOIActorProviderInterface * _actor ) const
	{
		NodeAOIActor * actor = static_cast<NodeAOIActor *>(_actor);

		const mt::vec3f & actor_pos = actor->getWorldPosition();
		
		const mt::vec3f & trigger_pos = this->getWorldPosition();
		
		float actor_radius = actor->getRadius();
		float trigger_radius = this->getRadius();

		float sqrradius = (actor_radius + trigger_radius) * (actor_radius + trigger_radius);
		float sqrlength = mt::sqrlength_v3_v3( trigger_pos, actor_pos );

		return sqrlength <= sqrradius;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeAOITrigger::onAOIActorEnter( AOIActorProviderInterface * _actor )
	{
		NodeAOIActor * actor = static_cast<NodeAOIActor *>(_actor);

		uint32_t actor_iff = actor->getIFF();

		EVENTABLE_CALL( m_serviceProvider, this, EVENT_TRIGGER_ENTER )(this, actor, m_iff, actor_iff);
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeAOITrigger::onAOIActorLeave( AOIActorProviderInterface * _actor )
	{
		NodeAOIActor * actor = static_cast<NodeAOIActor *>(_actor);

		uint32_t actor_iff = actor->getIFF();

		EVENTABLE_CALL( m_serviceProvider, this, EVENT_TRIGGER_LEAVE )(this, actor, m_iff, actor_iff);
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeAOITrigger::_activate()
	{
		if( m_aoi == nullptr )
		{
			return false;
		}

		m_trigger = m_aoi->createTrigger( this );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeAOITrigger::_deactivate()
	{
		m_aoi->removeTriger( m_trigger );
		m_trigger = nullptr;
	}
}