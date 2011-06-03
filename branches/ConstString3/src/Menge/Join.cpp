#	include "Join.h"

#	include "Node.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Join::Join( Node * _left, Node * _right )
		: m_left(_left)
		, m_right(_right)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Join::initialize()
	{
		m_leftPosition = m_left->getWorldPosition();
		m_rightPosition = m_right->getWorldPosition();

		m_offsetPosition = m_rightPosition - m_leftPosition;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Join::update()
	{
		const mt::vec2f & pos_left = m_left->getWorldPosition();
		const mt::vec2f & pos_right = m_right->getWorldPosition();

		float left_offset = mt::sqrlength_v2_v2(m_leftPosition, pos_left);
		float right_offset = mt::sqrlength_v2_v2(m_rightPosition, pos_right);
		
		if( left_offset > 0.25f && right_offset > 0.25f )
		{
			mt::vec2f center = (pos_left + pos_right) * 0.5f;

			m_leftPosition = center - m_offsetPosition * 0.5f;
			m_rightPosition = center + m_offsetPosition * 0.5f;

			m_left->setWorldPosition( m_leftPosition );
			m_right->setWorldPosition( m_rightPosition );

			return true;
		}
		else if( left_offset > 0.25f )
		{
			m_rightPosition = pos_left + m_offsetPosition;

			m_right->setWorldPosition( m_rightPosition );

			return true;
		}
		else if( right_offset > 0.25f )
		{
			m_leftPosition = pos_right - m_offsetPosition;

			m_left->setWorldPosition( m_leftPosition );

			return true;
		}

		return false;
	}
}