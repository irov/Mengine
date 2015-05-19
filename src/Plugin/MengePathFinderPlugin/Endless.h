#	pragma once

#	include "Kernel/Node.h"

#	include "pybind/object.hpp"

namespace Menge
{
	class Endless
	{
	public:
		Endless();
		~Endless();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider );

	public:
		bool initialize( uint32_t _countX, uint32_t _countY, float _width, float _height, bool _horizontalSlide, bool _verticalSlide, const pybind::object & _cb );
		void finalize();

	public:
		void slide( const mt::vec3f & _offset );

	protected:
		void slideRight_( uint32_t _begin, uint32_t _end );
		void slideLeft_( uint32_t _begin, uint32_t _end );
		void slideUp_( uint32_t _begin, uint32_t _end );
		void slideDown_( uint32_t _begin, uint32_t _end );

	protected:
		ServiceProviderInterface * m_serviceProvider;

		bool m_horizontSlide;
		bool m_verticalSlide;

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
