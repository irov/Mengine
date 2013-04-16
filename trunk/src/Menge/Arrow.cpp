#	include "Arrow.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/InputSystemInterface.h"

#	include "HotSpot.h"

#	include "Game.h"
#	include "Player.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Arrow::Arrow()
		: m_offsetClick(0.f, 0.f)
		, m_invalidateClickMatrix(true)
		, m_hided(false)
		, m_radius(0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::setOffsetClick( const mt::vec2f & _offsetClick )
	{
		m_offsetClick = _offsetClick;

		this->invalidateClickMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Arrow::getOffsetClick() const
	{
		return m_offsetClick;
	}
	//////////////////////////////////////////////////////////////////////////
	Scene * Arrow::getScene()
	{
		Scene * scene = PLAYER_SERVICE(m_serviceProvider)
			->getCurrentScene();

		return scene;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Arrow::_compile()
	{
		size_t num_points = boost::geometry::num_points(m_polygon);
		if( num_points == 0 )
		{
			Polygon polygon;
			boost::geometry::append( polygon, mt::vec2f(0.f,0.f) );

			this->setPolygon( polygon );			
		}		
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Arrow::_activate()
	{
		if( Entity::_activate() == false )
		{
			return false;
		}

		INPUT_SERVICE(m_serviceProvider)
			->addMousePositionProvider(this);

        const mt::vec2f & cursor_pos = INPUT_SERVICE(m_serviceProvider)
            ->getCursorPosition();

        mt::vec3f pos;
        pos.x = cursor_pos.x;
        pos.y = cursor_pos.y;
        pos.z = 0.f;

        this->setLocalPosition( pos );
        
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::_deactivate()
	{	
		INPUT_SERVICE(m_serviceProvider)
			->removeMousePositionProvider(this);

		Entity::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::onMousePositionChange( const mt::vec2f & _position )
	{
		mt::vec3f v3(_position.x, _position.y, 0.f);

		this->setLocalPosition( v3 );
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::addPoint_( const mt::vec2f & _v )
	{
		boost::geometry::append( m_polygon, _v );
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::setPolygon( const Polygon & _polygon )
	{
		m_polygon = _polygon;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::removePolygon()
	{
		Polygon polygon;
		boost::geometry::append( polygon, mt::vec2f(0.f,0.f) );

		this->setPolygon( polygon );		
	}
	//////////////////////////////////////////////////////////////////////////
	const Polygon & Arrow::getPolygon() const
	{
		return m_polygon;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::setRadius( float _radius )
	{
		m_radius = _radius;
	}
	//////////////////////////////////////////////////////////////////////////
	float Arrow::getRadius() const
	{
		return m_radius;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::onAppMouseLeave()
	{
		Node::hide( true );
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::onAppMouseEnter()
	{
		if( !m_hided )
		{
			Node::hide( false );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::hide( bool _value )
	{
		Node::hide( _value );
		m_hided = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::setContentResolution( const Resolution & _resolution )
	{
		m_contentResolution = _resolution;

		this->invalidateClickMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::setCurrentResolution( const Resolution & _resolution )
	{
		m_currentResolution = _resolution;

		this->invalidateClickMatrix_();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat3f & Arrow::getClickMatrix()
	{
		if( m_invalidateClickMatrix == true )
		{
			this->updateClickMatrix_();
		}

		return m_clickMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Arrow::getClickPosition()
	{
		const mt::mat3f & cm = this->getClickMatrix();

		return cm.v2.to_vec2f();
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::updateClickMatrix_()
	{
		m_invalidateClickMatrix = false;

		mt::ident_m3( m_clickMatrix );
		mt::translate_m3( m_clickMatrix, m_clickMatrix, m_offsetClick );

		mt::vec2f resolutionScale = m_contentResolution.getScale( m_currentResolution );

		mt::vec3f scale;
		scale.x = resolutionScale.x;
		scale.y = resolutionScale.y;
		scale.z = 1.f;
        
		mt::scale_m3( m_clickMatrix, scale );
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::invalidateClickMatrix_()
	{
		m_invalidateClickMatrix = true;
	}
    //////////////////////////////////////////////////////////////////////////
    void Arrow::_debugRender( RenderCameraInterface * _camera, unsigned int _debugMask )
    {
        if( ( _debugMask & MENGE_DEBUG_HOTSPOTS ) <= 0 )
        {
            return;
        }

        size_t numpoints = boost::geometry::num_points( m_polygon );

        if( numpoints == 0 )
        {
            return;
        }

        VectorVertices::invalidateVertices();

        VectorVertices::TVectorVertex2D & vertices = this->getVertices();

        if( vertices.empty() )
        {
            return;
        }

        RENDER_SERVICE(m_serviceProvider)
            ->addRenderObject2D( _camera, m_debugMaterial, NULL, 0, &(vertices[0]), vertices.size(), LPT_LINE );
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::_updateVertices( VectorVertices::TVectorVertex2D & _vertices, unsigned char _invalidate )
    {
        (void)_invalidate;

        size_t numpoints = boost::geometry::num_points(m_polygon);

        if( numpoints == 0 )
        {
            return;
        }

        _vertices.resize( numpoints + 1 );

        const mt::mat4f & worldMat = this->getWorldMatrix();

        const Polygon::ring_type & ring = m_polygon.outer();

        for( size_t i = 0; i < numpoints; ++i )
        {
            mt::vec2f trP;
            mt::mul_v2_m4( trP, ring[i], worldMat );

            _vertices[i].pos[0] = trP.x;
            _vertices[i].pos[1] = trP.y;
            _vertices[i].pos[2] = 0.f;
            //_vertices[i].pos[3] = 1.f;

            _vertices[i].color = 0x8080FFFF;

            _vertices[i].uv[0] = 0.f;
            _vertices[i].uv[1] = 0.f;

            _vertices[i].uv2[0] = 0.f;
            _vertices[i].uv2[1] = 0.f;
        }

        if( _vertices.size() > 1 )
        {
            std::copy( _vertices.begin(), _vertices.begin() + 1, _vertices.end() - 1 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Arrow::_invalidateWorldMatrix()
    {
        Node::_invalidateWorldMatrix();

        VectorVertices::invalidateVertices();
    }
}
