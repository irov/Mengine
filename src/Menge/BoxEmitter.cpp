#	include	"BoxEmitter.h"

#	include "ObjectImplement.h"

#	include "ResourceManager.h"

#	include "ResourceBoxEmitter.h"

#	include "XmlParser/XmlParser.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(BoxEmitter)
	//////////////////////////////////////////////////////////////////////////
	static float rand2(float min, float max)
	{
		float r = (float)rand() / RAND_MAX;
		return min + r * (max-min);
	}
	//////////////////////////////////////////////////////////////////////////
	BoxEmitter::BoxEmitter()
		: ParticleEmitter(1000)
		, m_life(0.5f,1.26f)
		, m_spread(1.96f)
		, m_position(300.f,300.f)
		, m_dir(0.f)
		, m_velocity(300.f,300.f)
	{}
	//////////////////////////////////////////////////////////////////////////
	bool BoxEmitter::_activate()
	{
		m_resource = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceBoxEmitter>( m_resourceName );

		if( m_resource == 0 )
		{
			return false;
		}

		BoxEmitterDesc& desc = m_resource->getDesc();

		m_life = desc.life;
		m_spread = desc.spread;
		m_position = desc.position;
		m_dir = desc.dir;
		m_velocity = desc.velocity;
	
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void BoxEmitter::_deactivate()
	{
		Holder<ResourceManager>::hostage()
			->releaseResource( m_resource );
	}
	//////////////////////////////////////////////////////////////////////////
	void BoxEmitter::loader(TiXmlElement * _xml)
	{
		XML_FOR_EACH_TREE(_xml)
		{
			XML_CHECK_VALUE_NODE( "ResourceBoxEmitter", "Name", m_resourceName );
		}

		NodeCore::loader(_xml);
	}
	//////////////////////////////////////////////////////////////////////////
	void BoxEmitter::emitt(float _dt, Particle * _particle)
	{
		_particle->m_timeToLive = 0.0f;
		_particle->m_totalTime = rand2(m_life.x, m_life.y);

		_particle->m_position = m_position + mt::vec2f(rand2(-1,1) * 40, rand2(-1,1) * 40);

		float ang = m_dir-1.57f+rand2(0,m_spread)-m_spread/2.0f;

		_particle->m_direction.x = cosf(ang);
		_particle->m_direction.y = sinf(ang);

		_particle->m_direction *= rand2(m_velocity.x, m_velocity.y);
	}
	//////////////////////////////////////////////////////////////////////////
}