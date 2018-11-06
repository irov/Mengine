#pragma once

#include "Kernel/Node.h"

#include "pybind/object.hpp"

namespace Mengine
{
	class Endless
	{
	public:
		Endless();
		~Endless();

	public:
		bool initialize( uint32_t _count, float _width, const mt::vec3f & _parallax, const pybind::object & _cb );
		void finalize();

	public:
		void slide( float _offset );

	protected:
		void slideRight_();
		void slideLeft_();

	protected:
		uint32_t m_elementCount;
		float m_elementWidth;

		pybind::object m_elementCb;

		float m_offset;

		struct Element
		{
			NodePtr node;
			uint32_t id;
		};

		typedef Vector<Element> TVectorElement;
		TVectorElement m_elements;

		uint32_t m_enumeratorElementId;
    };
}
