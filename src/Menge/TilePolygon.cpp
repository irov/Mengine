#	include "TilePolygon.h"

#	include "Light2D.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"

#	include "LightSystem.h"

#	include "RenderEngine.h"

#	include "math/triangulation.h"

#	include "ResourceManager.h"

#	include "ResourceImage.h"

#	include "LogEngine.h"

#	include "math/convexpoly2.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(TilePolygon);
	//////////////////////////////////////////////////////////////////////////
	TilePolygon::TilePolygon()
		: m_resourcename("")
		, m_resource(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::loader( XmlElement * _xml )
	{
		//RigidBody2D::loader(_xml);
		Node::loader(_xml);

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE("Vertex")
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE_MEMBER( "Value", &TilePolygon::_addVertex );
				}
			}

			XML_CASE_ATTRIBUTE_NODE( "ImageMap", "Name", m_resourcename );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::_render( const Viewport & _viewport, bool _enableDebug )
	{
		_renderPass( m_penumbra_triangles, 0xFFAAFFBB );
		_renderPass( m_triangles, 0xFFCFFFFF );
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::_renderPass( const std::vector<mt::vec2f> & _triangles, unsigned int _color )
	{
		for(std::vector<mt::vec2f>::size_type i = 0; i < _triangles.size(); i+=3)
		{
			const mt::vec2f & a = _triangles[i+0];
			const mt::vec2f & b = _triangles[i+1];
			const mt::vec2f & c = _triangles[i+2];

			const RenderImageInterface * image = m_resource->getImage(0);

			float w = image->getWidth();
			float h = image->getHeight();

			mt::vec2f uv0(a.x / w, a.y / h);
			mt::vec2f uv1(b.x / w, b.y / h);
			mt::vec2f uv2(c.x / w, c.y / h);

			const mt::mat3f & wm = getWorldMatrix();

			Holder<RenderEngine>::hostage()->renderTriple(wm, a, b, c,
				uv0, uv1, uv2,
				_color, image);
		}
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
		std::vector<mt::vec2f>::size_type size = m_poly.size();
		std::vector<mt::vec2f>::size_type capacity = 5 * size;

		m_triangles.reserve(capacity);
		m_penumbra_triangles.reserve(capacity);
		
		bool result = mt::triangulate_polygon(m_poly,m_triangles);

		if(result == false)
		{
			MENGE_LOG( "Error: can't triangulate polygon \n" );
			return false;
		}

		std::vector<mt::vec2f> contour;

		contour.reserve(capacity);
	
		result = mt::make_countour_polygon(m_poly,10,contour);

		if(result == false)
		{
			MENGE_LOG( "Error: can't create contour \n" );
			return false;
		}

		result = mt::triangulate_polygon(contour,m_penumbra_triangles);

		if(result == false)
		{
			MENGE_LOG( "Error: can't triangulate outline polygon \n" );
			return false;
		}

		m_resource = 
				Holder<ResourceManager>::hostage()
				->getResourceT<ResourceImage>( m_resourcename );

		if( m_resource == 0 )
		{
			MENGE_LOG( "Image resource not getting '%s'", m_resourcename.c_str() );
			return false;
		}

		//AGHTUNG!!!!!!!!!!!!!!!!!!!!!!!!!! int -> bool
		result = mt::decompose_concave(contour,polys);
	
		for(std::vector<mt::vec2f>::size_type i = 0; i < polys.size(); i++)
		{
			m_shapeList.push_back(polys[i]);
		}

		if(result == false)
		{
			MENGE_LOG( "Error: can't divide into polygons \n" );
			return false;
		}

		if( RigidBody2D::_compile() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::_release()
	{
		RigidBody2D::_release();

		Holder<ResourceManager>::hostage()->releaseResource( m_resource );

		m_resource = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::_update( float _timing )
	{
		RigidBody2D::_update(_timing);
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::_addVertex(const mt::vec2f & _vertex)
	{
		m_poly.push_back(_vertex);
	}
	//////////////////////////////////////////////////////////////////////////
}