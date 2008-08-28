#	include "TilePolygon.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"

#	include "LightSystem.h"

#	include "RenderEngine.h"

#	include "math/triangulation.h"

#	include "LogEngine.h"

#	include "math/convexpoly2.h"

#	include "math/angle.h"

//	resource section
#	include "ResourceManager.h"
#	include "ResourceImage.h"
#	include "ResourceTilePolygon.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(TilePolygon);
	//////////////////////////////////////////////////////////////////////////
	TilePolygon::TilePolygon()
		: m_resourcename( MENGE_TEXT("") )
		, m_tileResource( MENGE_TEXT("") )
		, m_juncName( MENGE_TEXT("") )
		, m_resource(0)
		, m_tilePolygonResource( 0 )
		, m_resourceJunc(0)
		, m_image(0)
		, m_imageJunc(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::loader( XmlElement * _xml )
	{
		Node::loader(_xml);

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( MENGE_TEXT("Vertex") )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE_MEMBER( MENGE_TEXT("Value"), &TilePolygon::addVertex );
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
	void TilePolygon::_render( unsigned int _debugMask )
	{
		Node::_render( _debugMask );

		const mt::mat3f & wm = getWorldMatrix();

		for(std::vector<mt::vec2f>::size_type i = 0; i < m_triangles.size(); i+=3)
		{
			const mt::vec2f & v0 = m_triangles[i+0];
			const mt::vec2f & v1 = m_triangles[i+1];
			const mt::vec2f & v2 = m_triangles[i+2];

			const mt::vec2f & uv0 = m_uvs[i+0];
			const mt::vec2f & uv1 = m_uvs[i+1];
			const mt::vec2f & uv2 = m_uvs[i+2];

			Holder<RenderEngine>::hostage()->renderTriple(wm, v0, v1, v2,
				uv0, uv1, uv2,
				0xFFFFFFFF, m_image );
		}

		for(std::list<Tile>::iterator it = m_tiles.begin(); it != m_tiles.end(); it++)
		{
			it->renderTile(wm,m_imageJunc);
		}

#	ifndef MENGE_MASTER_RELEASE
		if( _debugMask & MENGE_DEBUG_TILEPOLYGON )
		{
			int i = 0;
			for(std::list<Tile>::iterator it = m_tiles.begin(); it != m_tiles.end(); it++,i++)
			{
				it->renderDebug(wm, 0xFF00FF00 +  ((i+1)*105));
			}
		}
# endif
	}
	//////////////////////////////////////////////////////////////////////////
	bool TilePolygon::_activate()
	{
		if( RigidBody2D::_activate() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::_deactivate()
	{
		RigidBody2D::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool TilePolygon::_compile()
	{
		m_resource = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceImage>( m_resourcename );

		if( m_resource == 0 )
		{
			MENGE_LOG( MENGE_TEXT("Image resource not getting '%s'")
				,m_resourcename.c_str() );
			return false;
		}

		m_resourceJunc = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceImage>( m_juncName );

		if( m_resourceJunc == 0 )
		{
			MENGE_LOG( MENGE_TEXT("Image resource not getting '%s'")
				,m_juncName.c_str() );
			return false;
		}

		m_imageJunc = m_resourceJunc->getImage(0);

		std::vector<mt::vec2f>::size_type capacity = 5 * m_poly.size();

		m_triangles.reserve(capacity);
		m_uvs.reserve(capacity);
		
		bool result = mt::triangulate_polygon(m_poly,m_triangles);

		m_image = m_resource->getImage(0);

		float inv_width = 1.f / m_image->getWidth();
		float inv_height = 1.f / m_image->getHeight();

		for(mt::TVectorPoints::size_type i = 0; i < m_triangles.size(); i++)
		{
			mt::vec2f uv(m_triangles[i].x * inv_width, m_triangles[i].y * inv_height);
			m_uvs.push_back(uv);
		}

		if(result == false)
		{
			MENGE_LOG( MENGE_TEXT("Error: can't triangulate polygon \n") );
			return false;
		}

		result = mt::decompose_concave(m_poly,m_shapeList) > 0;
	
		if(result == false)
		{
			MENGE_LOG( MENGE_TEXT("Error: can't divide into polygons \n") );
			return false;
		}

		if(RigidBody2D::_compile() == false)
		{
			return false;
		}

		mt::vec2f juncSize = mt::vec2f(m_imageJunc->getWidth(),m_imageJunc->getHeight());

		for(std::list<Tile>::iterator it = m_tiles.begin(); it != m_tiles.end(); it++)
		{
			it->create(m_poly,juncSize);
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::_release()
	{
		RigidBody2D::_release();

		Holder<ResourceManager>::hostage()->releaseResource( m_resource );
		Holder<ResourceManager>::hostage()->releaseResource( m_resourceJunc );

		m_resource = NULL;
		m_resourceJunc = NULL;

		m_tiles.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::_update( float _timing )
	{
		RigidBody2D::_update(_timing);
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::_updateBoundingBox( mt::box2f& _boundingBox )
	{
		Node::_updateBoundingBox( _boundingBox );

		for(mt::TVectorPoints::size_type i = 0; i < m_triangles.size(); i++)
		{
			mt::add_internal_point( _boundingBox, m_triangles[i] );
		}

		for(std::list<Tile>::iterator it = m_tiles.begin(); it != m_tiles.end(); it++)
		{
			it->updateBoundingBox(_boundingBox);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::addVertex(const mt::vec2f & _vertex)
	{
		m_poly.push_back(_vertex);
	}
	//////////////////////////////////////////////////////////////////////////
}