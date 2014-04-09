#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "stdex/memorycopy.h"

namespace Menge
{
	template<size_t VerticesCount>
	class FixedVertices
	{
	public:
		FixedVertices()
			: m_invalidateVertices(0xFF)
		{
		}

	public:
		void setVertices( const RenderVertex2D * _vertices )
		{
			stdex::memorycopy( m_vertices, _vertices, VerticesCount );
		}

		inline const RenderVertex2D * getVertices()
		{
			if( m_invalidateVertices != 0 )
			{
				updateVertices_();
			}

			return m_vertices;
		}

		inline void invalidateVertices( unsigned char _invalidate = 0xFF )
		{
			m_invalidateVertices |= _invalidate;
		}

	private:
		inline void updateVertices_()
		{
			this->_updateVertices( m_vertices, m_invalidateVertices );

			m_invalidateVertices = 0;
		}
	
	protected:
		virtual void _updateVertices( RenderVertex2D * _vertices, unsigned char _invalidateVertices ) = 0;

	private:
		RenderVertex2D m_vertices[VerticesCount];
		unsigned char m_invalidateVertices;
	};

	typedef FixedVertices<4> QuadVertices;
}
