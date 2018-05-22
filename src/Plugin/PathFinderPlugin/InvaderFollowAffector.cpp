#include "InvaderFollowAffector.h"

#include "math/angle.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	InvaderFollowAffector::InvaderFollowAffector()
		: m_moveSpeed( 0.f )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	InvaderFollowAffector::~InvaderFollowAffector()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const NodePtr & InvaderFollowAffector::getNode() const
	{
		return m_node;
	}
	//////////////////////////////////////////////////////////////////////////
    const NodePtr & InvaderFollowAffector::getTarget() const
	{
		return m_target;
	}
	//////////////////////////////////////////////////////////////////////////
	void InvaderFollowAffector::setMoveSpeed( float _moveSpeed )
	{
		m_moveSpeed = _moveSpeed;
	}
	//////////////////////////////////////////////////////////////////////////
	float InvaderFollowAffector::getMoveSpeed() const
	{
		return m_moveSpeed;
	}
	//////////////////////////////////////////////////////////////////////////
	bool InvaderFollowAffector::initialize( const NodePtr & _node, const NodePtr & _target, const pybind::object & _cb, const pybind::args & _args )
	{
		if( _node == nullptr )
		{
			return false;
		}

		if( _target == nullptr )
		{
			return false;
		}

		m_node = _node;
		m_target = _target;

		m_cb = _cb;
		m_args = _args;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool InvaderFollowAffector::_affect( float _current, float _timing )
	{
        (void)_current;

		mt::vec3f node_position = m_node->getLocalPosition();
		mt::vec3f follow_position = m_target->getLocalPosition();

		mt::vec3f current_direction;
		float length = mt::dir_v3_v3_f( current_direction, node_position, follow_position );

		if( length < mt::constant::eps )
		{
			return false;
		}

		float step = m_moveSpeed * _timing;

		if( length - step < 0.f )
		{
			if( m_cb.is_callable() == true )
			{
				m_cb.call_args( this, current_direction, length, true, m_args );
			}

			m_node->setLocalPosition( follow_position );
		}
		else
		{
			if( m_cb.is_callable() == true )
			{
				m_cb.call_args( this, current_direction, step, false, m_args );
			}
			
			mt::vec3f new_position = node_position + current_direction * step;

			m_node->setLocalPosition( new_position );
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void InvaderFollowAffector::complete()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void InvaderFollowAffector::stop()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * InvaderFollowAffector::_embedded()
	{
		pybind::kernel_interface * kernel = pybind::get_kernel();

		PyObject * py_obj = kernel->scope_create_holder_t( this );

		return py_obj;
	}
}

