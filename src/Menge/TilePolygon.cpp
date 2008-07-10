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
	bool TilePolygon::isVisible( const Viewport & _viewPort )
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::loader( XmlElement * _xml )
	{
		SceneNode2D::loader(_xml);

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
	void TilePolygon::_render( bool _enableDebug )
	{
		_renderPass(m_penumbra_triangles, 0xFFAAFFBB);
		_renderPass(m_triangles, 0xFFFFFFFF);
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::_renderPass( const std::vector<mt::vec2f> & _triangles, unsigned int _color )
	{
		for(int i = 0; i < _triangles.size(); i+=3)
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
		if( SceneNode2D::_activate() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::_deactivate()
	{
		SceneNode2D::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool TilePolygon::_compile()
	{
		if( SceneNode2D::_compile() == false )
		{
			return false;
		}

		int size = m_poly.size();
		int capacity = 5 * size;

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

		//test
		//setLocalPosition(mt::vec2f(200,50));

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::_release()
	{
		SceneNode2D::_release();

		Holder<ResourceManager>::hostage()->releaseResource( m_resource );

		m_resource = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::_update( float _timing )
	{
		SceneNode2D::_update(_timing);
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::_addVertex(const mt::vec2f & _vertex)
	{
		m_poly.push_back(_vertex);
	}
	//////////////////////////////////////////////////////////////////////////
}