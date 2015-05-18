#	pragma once

#	include "Kernel/Node.h"

#	include "pybind/object.hpp"

namespace Menge
{
	class Endless
		: public Node
	{
	public:
		Endless();
		~Endless();

	public:
		void setElementCount( uint32_t _countX, uint32_t _countY );
		void setElementSize( float _width, float _height );
		void setElementCb( const pybind::object & _cb );

	public:
		bool _activate() override;
		void _deactivate() override;
 
	public:
		void slide( const mt::vec3f & _offset );

	protected:
		void slideRight_( uint32_t _begin, uint32_t _end );
		void slideLeft_( uint32_t _begin, uint32_t _end );
		void slideUp_( uint32_t _begin, uint32_t _end );
		void slideDown_( uint32_t _begin, uint32_t _end );

	protected:
		uint32_t m_elementCountX;
		uint32_t m_elementCountY;
		float m_elementWidth;
		float m_elementHeight;

		pybind::object m_elementCb;

		mt::vec3f m_offset;

		typedef stdex::vector<Node *> TVectorNode;
		TVectorNode m_nodes;
    };
}
