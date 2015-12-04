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
		bool initialize( uint32_t _count, float _width, bool _slide, const pybind::object & _cb );
		void finalize();

	public:
		void slide( float _offset );

	protected:
		void slideRight_();
		void slideLeft_();

	protected:
		ServiceProviderInterface * m_serviceProvider;

		uint32_t m_elementCount;
		float m_elementWidth;
		bool m_elementSlide;

		pybind::object m_elementCb;

		float m_offset;

		struct Element
		{
			Node * node;
			uint32_t id;
		};

		typedef stdex::vector<Element> TVectorElement;
		TVectorElement m_elements;

		uint32_t m_enumeratorElementId;
    };
}
