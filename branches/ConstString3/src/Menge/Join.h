#	pragma once

#	include "Math/vec2.h"

namespace Menge
{
	class Node;

	class Join
	{
	public:
		Join( Node * _left, Node * _right, const mt::vec2f & _offset );

	public:
		void initialize();

	public:
		Node * getLeft() const;
		Node * getRight() const;

	public:
		const mt::vec2f & getOffset() const;

	public:
		bool update();

	protected:
		Node * m_left;
		Node * m_right;

		mt::vec2f m_leftPosition;
		mt::vec2f m_rightPosition;
		mt::vec2f m_offset;
	};
}