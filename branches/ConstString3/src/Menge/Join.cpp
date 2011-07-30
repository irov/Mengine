#	include "Join.h"

#	include "Node.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Join::Join( Node * _left, Node * _right, const mt::vec2f & _offset )
		: m_left(_left)
		, m_right(_right)
		, m_offset(_offset)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Join::initialize()
	{
		m_leftPosition = m_left->getLocalPosition();
		m_rightPosition = m_right->getLocalPosition();

		mt::vec2f offset = m_rightPosition - m_leftPosition;

		float offset_sqr = sqrlength_v2_v2(offset, m_offset);

		if( offset_sqr > 0.25f )
		{
			mt::vec2f rightPosition = m_leftPosition + m_offset;
			m_right->setLocalPosition( rightPosition );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Node * Join::getLeft() const
	{
		return m_left;
	}
	//////////////////////////////////////////////////////////////////////////
	Node * Join::getRight() const
	{
		return m_right;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Join::getOffset() const
	{
		return m_offset;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Join::update()
	{
		const mt::vec2f & pos_left = m_left->getLocalPosition();
		const mt::vec2f & pos_right = m_right->getLocalPosition();

		mt::vec2f offset = pos_right - pos_left;

		float offset_sqr = sqrlength_v2_v2(offset, m_offset);
		
		if( offset_sqr < 0.25f )
		{
			return false;
		}

		float left_offset = mt::sqrlength_v2_v2(m_leftPosition, pos_left);
		float right_offset = mt::sqrlength_v2_v2(m_rightPosition, pos_right);
				
		if( left_offset > 0.25f && right_offset > 0.25f )
		{
			mt::vec2f center = (pos_left + pos_right) * 0.5f;

			m_leftPosition = center - m_offset * 0.5f;
			m_rightPosition = center + m_offset * 0.5f;

			m_left->setLocalPosition( m_leftPosition );
			m_right->setLocalPosition( m_rightPosition );

			return true;
		}
		else if( left_offset > 0.25f )
		{
			m_leftPosition = pos_left;
			m_rightPosition = pos_left + m_offset;

			m_right->setLocalPosition( m_rightPosition );

			return true;
		}
		else if( right_offset > 0.25f )
		{
			m_rightPosition = pos_right;
			m_leftPosition = pos_right - m_offset;

			m_left->setLocalPosition( m_leftPosition );

			return true;
		}

		return false;
	}
}