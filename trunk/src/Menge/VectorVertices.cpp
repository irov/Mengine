#	include "VectorVertices.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	VectorVertices::VectorVertices()
		: m_invalidateVertices(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void VectorVertices::updateVertices_()
	{
		_updateVertices( m_vertices, m_invalidateVertices );
		m_invalidateVertices = 0;
	}
}
