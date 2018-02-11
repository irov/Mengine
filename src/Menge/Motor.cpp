#	include "Motor.h"

#	include "Kernel/Node.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Motor::Motor()
		: m_node(nullptr)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	Motor::~Motor()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Motor::setNode(Node * _node)
	{
		m_node = _node;
	}
	//////////////////////////////////////////////////////////////////////////
	Node * Motor::getNode() const
	{
		return m_node;
	}
	//////////////////////////////////////////////////////////////////////////
	void Motor::addVelocity(const ConstString & _name, const mt::vec3f & _velocity)
	{
		VelocityDesc & desc = m_velocities[_name];

		desc.velocity = _velocity;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Motor::hasVelocity(const ConstString & _name) const
	{
		TMapVelocity::const_iterator it_found = m_velocities.find(_name);

		if(it_found == m_velocities.end())
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec3f Motor::getVelocity(const ConstString & _name) const
	{
		TMapVelocity::const_iterator it_found = m_velocities.find(_name);

		if(it_found == m_velocities.end())
		{
			return mt::vec3f(0.f, 0.f, 0.f);
		}

		const VelocityDesc & desc = it_found->second;

		return desc.velocity;
	}
	//////////////////////////////////////////////////////////////////////////
	void Motor::_update(float _current, float _timing)
	{
		(void)_current;

		if(m_node == nullptr)
		{
			return;
		}

		mt::vec3f velocity(0.f, 0.f, 0.f);

		for(TMapVelocity::const_iterator
			it = m_velocities.begin(),
			it_end = m_velocities.end();
		it != it_end;
		++it)
		{
			const VelocityDesc & desc = it->second;

			velocity += desc.velocity * _timing;
		}

		m_node->translate(velocity);
	}
}