#pragma once

#include "Core/RenderVertex2D.h"

#include "stdex/stl_vector.h"

namespace Mengine
{
	class VectorVertices
	{
	public:
		VectorVertices();
        
	public:
		inline TVectorRenderVertex2D & getVertices();
		inline void invalidateVertices( unsigned char _invalidate = 0xFE );

	protected:
		void updateVertices_();
	
	protected:
		virtual void _updateVertices( TVectorRenderVertex2D & _vertices, unsigned char _invalidate ) = 0;

	private:
        TVectorRenderVertex2D m_vertices;
		unsigned char m_invalidateVertices;
	};
	//////////////////////////////////////////////////////////////////////////
	inline TVectorRenderVertex2D & VectorVertices::getVertices()
	{
		if( m_invalidateVertices != 0 )
		{
			_updateVertices( m_vertices, m_invalidateVertices );
		}

		return m_vertices;
	}
	//////////////////////////////////////////////////////////////////////////
	inline void VectorVertices::invalidateVertices( unsigned char _invalidate )
	{
		m_invalidateVertices |= _invalidate;
	}
}
