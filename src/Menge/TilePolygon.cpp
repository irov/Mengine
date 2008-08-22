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
		: m_resourcename( MENGE_TEXT("") )
		, m_resource(0)
		, m_imagePenumbraUp( 0 )
		, m_imagePenumbraDown( 0 )
		, m_imagePenumbraLeft( 0 )
		, m_imagePenumbraRight( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::loader( XmlElement * _xml )
	{
		//RigidBody2D::loader(_xml);
		Node::loader(_xml);

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( MENGE_TEXT("Vertex") )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE_MEMBER( MENGE_TEXT("Value"), &TilePolygon::_addVertex );
				}
			}

			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("ImageMap"), MENGE_TEXT("Name"), m_resourcename );
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("PenumbraUp"), MENGE_TEXT("Name"), m_penumbraUpName );
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("PenumbraDown"), MENGE_TEXT("Name"), m_penumbraDownName );
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("PenumbraLeft"), MENGE_TEXT("Name"), m_penumbraLeftName );
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("PenumbraRight"), MENGE_TEXT("Name"), m_penumbraRightName );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::_render( unsigned int _debugMask )
	{
		Node::_render( _debugMask );
		for(std::vector<mt::vec2f>::size_type i = 0; i < m_triangles.size(); i+=3)
		{
			const mt::vec2f & a = m_triangles[i+0];
			const mt::vec2f & b = m_triangles[i+1];
			const mt::vec2f & c = m_triangles[i+2];

			const RenderImageInterface * image = m_resource->getImage(0);

			float w = image->getWidth();
			float h = image->getHeight();

			mt::vec2f uv0(a.x / w, a.y / h);
			mt::vec2f uv1(b.x / w, b.y / h);
			mt::vec2f uv2(c.x / w, c.y / h);

			const mt::mat3f & wm = getWorldMatrix();

			Holder<RenderEngine>::hostage()->renderTriple(wm, a, b, c,
				uv0, uv1, uv2,
				0xFFFFFFFF, image );
		}

		if( m_imagePenumbraUp != 0 )
		{
			renderEdges_( m_edgesUp, m_imagePenumbraUp->getImage( 0 ) );
		}
		if( m_imagePenumbraDown != 0 )
		{
			renderEdges_( m_edgesDown, m_imagePenumbraDown->getImage( 0 ) );
		}
		if( m_imagePenumbraLeft != 0 )
		{
			renderEdges_( m_edgesLeft, m_imagePenumbraLeft->getImage( 0 ) );
		}
		if( m_imagePenumbraRight != 0 )
		{
			renderEdges_( m_edgesRight, m_imagePenumbraRight->getImage( 0 ) );
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
		m_resource = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceImage>( m_resourcename );

		if( m_resource == 0 )
		{
			MENGE_LOG( MENGE_TEXT("Image resource not getting '%s'")
				,m_resourcename.c_str() );
			return false;
		}

		if( m_penumbraUpName != MENGE_TEXT("") )
		{
			m_imagePenumbraUp = 
				Holder<ResourceManager>::hostage()
				->getResourceT<ResourceImage>( m_penumbraUpName );
			float width = m_imagePenumbraUp->getImage( 0 )->getWidth();
			float height = m_imagePenumbraUp->getImage( 0 )->getHeight();
			compileEdges_( width, height, -0.261f, 0.261f, m_edgesUp );
		}
		if( m_penumbraDownName != MENGE_TEXT("") )
		{
			m_imagePenumbraDown =
				Holder<ResourceManager>::hostage()
				->getResourceT<ResourceImage>( m_penumbraDownName );
			float width = m_imagePenumbraDown->getImage( 0 )->getWidth();
			float height = m_imagePenumbraUp->getImage( 0 )->getHeight();
			compileEdges_( width, height, 2.88f, 3.40f, m_edgesDown );
		}
		if( m_penumbraLeftName != MENGE_TEXT("") )
		{
			m_imagePenumbraLeft =
				Holder<ResourceManager>::hostage()
				->getResourceT<ResourceImage>( m_penumbraLeftName );
			float width = m_imagePenumbraLeft->getImage( 0 )->getWidth();
			float height = m_imagePenumbraUp->getImage( 0 )->getHeight();
			compileEdges_( width, height, 1.309f, 1.833f, m_edgesLeft );
		}

		if( m_penumbraRightName != MENGE_TEXT("") )
		{
			m_imagePenumbraRight =
				Holder<ResourceManager>::hostage()
				->getResourceT<ResourceImage>( m_penumbraRightName );
			float width = m_imagePenumbraRight->getImage( 0 )->getWidth();
			float height = m_imagePenumbraUp->getImage( 0 )->getHeight();
			compileEdges_( width, height, -1.833f, -1.309f, m_edgesRight );
		}

		std::vector<mt::vec2f>::size_type size = m_poly.size();
		std::vector<mt::vec2f>::size_type capacity = 5 * size;

		m_triangles.reserve(capacity);
		
		bool result = mt::triangulate_polygon(m_poly,m_triangles);

		if(result == false)
		{
			MENGE_LOG( MENGE_TEXT("Error: can't triangulate polygon \n") );
			return false;
		}

		std::vector<mt::vec2f> contour;

		contour.reserve(capacity);
	
		if(result == false)
		{
			MENGE_LOG( MENGE_TEXT("Error: can't create contour \n") );
			return false;
		}

		if(result == false)
		{
			MENGE_LOG( MENGE_TEXT("Error: can't triangulate outline polygon \n") );
			return false;
		}

		mt::decompose_concave(m_poly,polys);
	
		for(std::vector<mt::vec2f>::size_type i = 0; i < polys.size(); i++)
		{
			m_shapeList.push_back(polys[i]);
		}

		if(result == false)
		{
			MENGE_LOG( MENGE_TEXT("Error: can't divide into polygons \n") );
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
		if( m_imagePenumbraUp )
		{
			Holder<ResourceManager>::hostage()->releaseResource( m_imagePenumbraUp );
		}
		if( m_imagePenumbraDown )
		{
			Holder<ResourceManager>::hostage()->releaseResource( m_imagePenumbraDown );
		}
		if( m_imagePenumbraLeft )
		{
			Holder<ResourceManager>::hostage()->releaseResource( m_imagePenumbraLeft );
		}
		if( m_imagePenumbraRight )
		{
			Holder<ResourceManager>::hostage()->releaseResource( m_imagePenumbraRight );
		}


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
	void TilePolygon::_updateBoundingBox( mt::box2f& _boundingBox )
	{
		Node::_updateBoundingBox( _boundingBox );
		for( mt::TVectorPoints::iterator it = m_triangles.begin(), it_end = m_triangles.end();
			it != it_end;
			it++ )
		{
			mt::add_internal_point( _boundingBox, (*it) );
		}

		for( std::vector<TOutQuad>::iterator it = m_edgesUp.begin(), it_end = m_edgesUp.end();
			it != it_end;
			it ++ )
		{
			mt::merge_box( _boundingBox, mt::box2f( it->first[0], it->first[2] ) );
		}

		for( std::vector<TOutQuad>::iterator it = m_edgesDown.begin(), it_end = m_edgesDown.end();
			it != it_end;
			it ++ )
		{
			mt::merge_box( _boundingBox, mt::box2f( it->first[0], it->first[2] ) );
		}

		/*for( std::vector<mt::vec2f>::iterator it = m_penumbra_triangles.begin(), it_end = m_penumbra_triangles.end();
			it != it_end;
			it++ )
		{
			mt::add_internal_point( _boundingBox, (*it) );
		}*/

	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::compileEdges_( float _width, float _height, float _minAngle, float _maxAngle, std::vector<TOutQuad>& _edges )
	{
		mt::TVectorPoints::size_type pointsNum = m_poly.size();

		// temporary add first point to end
		m_poly.push_back( m_poly[0] );
		mt::TVectorPoints edge;
		for( mt::TVectorPoints::size_type i = 0; i < pointsNum; i++ )
		{
			mt::vec2f vec = ( m_poly[i+1] - m_poly[i] );
			float len = vec.length();
			if( len < 0.00001f )
			{
				return;
			}
			float cos = vec.x / len;
			float angle = ::acosf( cos );
			if( (angle < 0.0f) && (_minAngle > 0.0f) )
			{
				angle += mt::m_pi * 2.0f;
			}
			if( (angle > _minAngle) && (angle < _maxAngle) )
			{
				mt::vec2f normal = mt::perp( vec ) / len;
				edge.push_back( mt::vec2f( m_poly[i].x, m_poly[i].y ) - normal * _height * 0.5f );
				edge.push_back( mt::vec2f( m_poly[i+1].x, m_poly[i+1].y ) - normal * _height * 0.5f );
				edge.push_back( mt::vec2f( m_poly[i+1].x, m_poly[i+1].y ) + normal * _height * 0.5f );
				edge.push_back( mt::vec2f( m_poly[i].x, m_poly[i].y ) + normal * _height * 0.5f );
				mt::vec2f tex( mt::length_v2_v2( edge[0], edge[1] ) / _width,
					mt::length_v2_v2( edge[1], edge[2] ) / _height );

				_edges.push_back( std::make_pair( edge, tex ) );
				edge.clear();
			}
		}

		// remove last point
		m_poly.pop_back();
	}
	//////////////////////////////////////////////////////////////////////////
	void TilePolygon::renderEdges_( const std::vector<TOutQuad>& _edges, const RenderImageInterface* _image )
	{
		for( std::vector<TOutQuad>::size_type j = 0; j < _edges.size(); j++ )
		{
			mt::TVectorPoints edge = _edges[j].first;
			const mt::vec2f & a = edge[0];
			const mt::vec2f & b = edge[1];
			const mt::vec2f & c = edge[2];
			const mt::vec2f & d = edge[3];

			float sx = _edges[j].second.x;
			float sy = _edges[j].second.y;
			mt::vec2f uva( 0.0f, 0.0f );
			mt::vec2f uvb( sx, 0.0f );
			mt::vec2f uvc( sx, sy );
			mt::vec2f uvd( 0.0f, sy );

			const mt::mat3f & wm = getWorldMatrix();

			Holder<RenderEngine>::hostage()->renderTriple(wm, a, b, d,
				uva, uvb, uvd,
				0xFFFFFFFF, _image );

			Holder<RenderEngine>::hostage()->renderTriple(wm, b, c, d,
				uvb, uvc, uvd,
				0xFFFFFFFF, _image );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}