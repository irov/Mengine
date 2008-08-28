#	include "ResourceTilePolygon.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

#	include "LogEngine.h"

#	include "math/triangulation.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceTilePolygon );
	//////////////////////////////////////////////////////////////////////////
	ResourceTilePolygon::ResourceTilePolygon( const ResourceFactoryParam & _params )
		: ResourceReference( _params )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceTilePolygon::loader( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( MENGE_TEXT("Vertex") )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE_MEMBER( MENGE_TEXT("Value"), &ResourceTilePolygon::addVertex );
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceTilePolygon::addVertex(const mt::vec2f & _vertex)
	{
		m_poly.push_back(_vertex);
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::TVectorPoints & ResourceTilePolygon::getTriangles() const
	{
		return m_triangles;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceTilePolygon::_compile()
	{
		mt::TVectorPoints::const_iterator it = m_triangles.begin();

		std::vector<mt::vec2f>::size_type size = m_poly.size();
		std::vector<mt::vec2f>::size_type capacity = 5 * size;

		m_triangles.reserve(capacity);

		bool result = mt::triangulate_polygon(m_poly,m_triangles);

		if(result == false)
		{
			MENGE_LOG( MENGE_TEXT("Error: can't triangulate polygon \n") );
			return false;
		}

		mt::decompose_concave(m_poly,m_concaves);

		if(result == false)
		{
			MENGE_LOG( MENGE_TEXT("Error: can't divide into polygons \n") );
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceTilePolygon::_release()
	{
	}
	//////////////////////////////////////////////////////////////////////////
}