#	include "ResourceTilePolygon.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

#	include "LogEngine.h"

#	include "math/triangulation.h"

#	include "ResourceImage.h"

#	include "Interface/RenderSystemInterface.h"

#	include "ResourceManager.h"

#	include "math/angle.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT(ResourceTilePolygon);
	//////////////////////////////////////////////////////////////////////////
	ResourceTilePolygon::ResourceTilePolygon(const ResourceFactoryParam & _params)
		: ResourceReference(_params)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceTilePolygon::loader(XmlElement * _xml)
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

			XML_CASE_NODE( MENGE_TEXT("Tile") )
			{
				float minAngle;
				float maxAngle;
				String imageName;

				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( MENGE_TEXT("MinAngle"), minAngle );
					XML_CASE_ATTRIBUTE( MENGE_TEXT("MaxAngle"), maxAngle );
					XML_CASE_ATTRIBUTE( MENGE_TEXT("Image"), imageName );
				}

				m_tiles.push_back(Tile(minAngle,maxAngle,imageName));
			}

			XML_CASE_NODE( MENGE_TEXT("TileJunc") )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( MENGE_TEXT("Image"), m_juncName );
				}
			}

			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("ImageMap"), MENGE_TEXT("Name"), m_resourcename );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceTilePolygon::addVertex(const mt::vec2f & _vertex)
	{
		m_poly.push_back(_vertex);
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderImageInterface * ResourceTilePolygon::getImage() const
	{
		return m_image;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderImageInterface * ResourceTilePolygon::getTileImage(unsigned int _tile) const
	{
		return m_tiles[_tile].m_image;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::TVectorPoints * ResourceTilePolygon::getTriangles() const
	{
		return &m_triangles;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::TVectorPoints * ResourceTilePolygon::getTextureCoords() const
	{
		return &m_uvs;
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceTilePolygon::getMinAngle(unsigned int _tile) const
	{
		return m_tiles[_tile].m_minAngle;
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceTilePolygon::getMaxAngle(unsigned int _tile) const
	{
		return m_tiles[_tile].m_maxAngle;
	}
	//////////////////////////////////////////////////////////////////////////
	const TListQuad * ResourceTilePolygon::getTileGeometry(unsigned int _tile) const
	{
		return &m_tiles[_tile].m_geometry;
	}
	//////////////////////////////////////////////////////////////////////////
	const TListQuad * ResourceTilePolygon::getTileJunks(unsigned int _tile) const
	{
		return &m_tiles[_tile].m_junks;
	}
	//////////////////////////////////////////////////////////////////////////
	const TListConcavePolygon * ResourceTilePolygon::getShapeList() const
	{
		return &m_concaves;
	}	
	//////////////////////////////////////////////////////////////////////////
	const RenderImageInterface * ResourceTilePolygon::getPlugImage() const
	{
		return m_imageJunc;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceTilePolygon::getTileCount() const
	{
		return m_tiles.size();
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceTilePolygon::_release()
	{
		Holder<ResourceManager>::hostage()->releaseResource( m_resource );
		m_resource = 0;

		Holder<ResourceManager>::hostage()->releaseResource( m_resourceJunc );
		m_resourceJunc = 0;

		for(std::list<ResourceImage*>::iterator it = m_imageResources.begin(); it != m_imageResources.end(); it++)
		{
			Holder<ResourceManager>::hostage()->releaseResource(*it);
		}

		m_imageResources.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceTilePolygon::_compile()
	{
		m_resource = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceImage>( m_resourcename );

		if(m_resource == 0)
		{
			MENGE_LOG_ERROR( MENGE_TEXT("Image resource not getting '%s'")
				,m_resourcename.c_str() );
			return false;
		}

		m_resourceJunc = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceImage>( m_juncName );

		if(m_resourceJunc == 0)
		{
			MENGE_LOG_ERROR( MENGE_TEXT("Image resource not getting '%s'")
				,m_juncName.c_str() );
			return false;
		}

		m_imageJunc = m_resourceJunc->getImage(0);

		mt::TVectorPoints::size_type size = m_poly.size();
		mt::TVectorPoints::size_type capacity = 5 * size;

		m_triangles.reserve(capacity);
		m_uvs.reserve(capacity);

		bool result = mt::triangulate_polygon(m_poly,m_triangles);

		if(result == false)
		{
			MENGE_LOG_ERROR( MENGE_TEXT("Error: can't triangulate polygon \n") );
			return false;
		}

		m_image = m_resource->getImage(0);

		float inv_width = 1.f / m_image->getWidth();
		float inv_height = 1.f / m_image->getHeight();

		for(mt::TVectorPoints::size_type i = 0; i < m_triangles.size(); i++)
		{
			mt::vec2f uv(m_triangles[i].x * inv_width, m_triangles[i].y * inv_height);
			m_uvs.push_back(uv);
		}

		mt::decompose_concave(m_poly,m_concaves);

		if(result == false)
		{
			MENGE_LOG_ERROR( MENGE_TEXT("Error: can't divide into polygons \n") );
			return false;
		}

		mt::vec2f juncSize = mt::vec2f(m_imageJunc->getWidth(),m_imageJunc->getHeight());

		// FIXXXXXXX
		for(std::vector<Tile>::iterator it = m_tiles.begin(); it != m_tiles.end(); it++)
		{
			create(*it,m_poly,juncSize);
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceTilePolygon::create(Tile & _tile, const mt::TVectorPoints & _poly, const mt::vec2f & _juncSize)
	{
		ResourceImage * m_imageResource =
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceImage>( _tile.m_imagename );

		m_imageResources.push_back(m_imageResource);

		 _tile.m_image = m_imageResource->getImage(0);

		float _width = _tile.m_image->getWidth();
		float _height = _tile.m_image->getHeight();

		mt::vec2f edge;
		mt::vec2f penumbra;

		bool found_junction = false;

		mt::vec2f track_edge;

		mt::TVectorPoints::size_type pointsNum = _poly.size();

		Quad quad;
		quad.v.resize(4);

		int track_point = 0;

		for(mt::TVectorPoints::size_type i = 0; i < pointsNum; i++)
		{
			int next_i = (i + 1) % pointsNum;

			edge = _poly[next_i] - _poly[i];

			float angle = mt::signed_angle(edge) * mt::m_rad2deg;

			if(mt::angle_in_interval_deg(angle,_tile.m_minAngle,_tile.m_maxAngle))
			{
				track_point = next_i;
				track_edge = mt::perp(edge);

				penumbra = track_edge / edge.length() * _height * 0.5f;

				quad.v[0] = _poly[i] - penumbra;
				quad.v[1] = _poly[next_i] - penumbra;
				quad.v[2] = _poly[next_i] + penumbra;
				quad.v[3] = _poly[i] + penumbra;

				quad.s = edge.length() / _width;
				quad.t = mt::length_v2_v2(quad.v[1], quad.v[2]) / _height;

				_tile.m_geometry.push_back(quad);

				found_junction = true;

				if(i < pointsNum-1)
					continue;
			}

			if(found_junction)
			{
				mt::vec2f normal = (track_edge + mt::perp(edge));
				normal = normal / normal.length();

				penumbra = normal * _juncSize.y * 0.5f;

				mt::vec2f lpoint = _poly[track_point] - mt::perp(normal) * _juncSize.x * 0.5f;
				mt::vec2f rpoint = _poly[track_point] + mt::perp(normal) * _juncSize.x * 0.5f;

				quad.v[0] = lpoint - penumbra;
				quad.v[1] = rpoint - penumbra;
				quad.v[2] = rpoint + penumbra;
				quad.v[3] = lpoint + penumbra;

				quad.s = mt::length_v2_v2(quad.v[0], quad.v[1]) / _juncSize.x;
				quad.t = mt::length_v2_v2(quad.v[1], quad.v[2]) / _juncSize.y;


				_tile.m_junks.push_back(quad);

				found_junction = false;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
}