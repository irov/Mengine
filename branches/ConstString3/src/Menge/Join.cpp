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
		m_leftPosition = m_left->getLocalPosition().to_vec2f();
		m_rightPosition = m_right->getLocalPosition().to_vec2f();

		mt::vec2f offset = m_rightPosition - m_leftPosition;

		float offset_sqr = sqrlength_v2_v2(offset, m_offset);

		if( offset_sqr > 0.25f )
		{
			mt::vec2f rightPosition = m_leftPosition + m_offset;
			mt::vec3f v3( rightPosition.x, rightPosition.y, 0.f );
			m_right->setLocalPosition( v3 );
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
		const mt::vec2f & pos_left = m_left->getLocalPosition().to_vec2f();
		const mt::vec2f & pos_right = m_right->getLocalPosition().to_vec2f();

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

			mt::vec3f v3_leftPosition( m_leftPosition.x, m_leftPosition.y, 0.f );
			m_left->setLocalPosition( v3_leftPosition );

			mt::vec3f v3_rightPosition( m_rightPosition.x, m_rightPosition.y, 0.f );
			m_right->setLocalPosition( v3_rightPosition );

			return true;
		}
		else if( left_offset > 0.25f )
		{
			m_leftPosition = pos_left;
			m_rightPosition = pos_left + m_offset;

			mt::vec3f v3_rightPosition( m_rightPosition.x, m_rightPosition.y, 0.f );
			m_right->setLocalPosition( v3_rightPosition );

			return true;
		}
		else if( right_offset > 0.25f )
		{
			m_rightPosition = pos_right;
			m_leftPosition = pos_right - m_offset;

			mt::vec3f v3_leftPosition( m_leftPosition.x, m_leftPosition.y, 0.f );
			m_left->setLocalPosition( v3_leftPosition );

			return true;
		}

		return false;
	}
}