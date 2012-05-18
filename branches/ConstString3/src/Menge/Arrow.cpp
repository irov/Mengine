#	include "Arrow.h"

#	include "Application.h"

#	include "RenderEngine.h"

#	include "InputEngine.h"

#	include "HotSpot.h"

#	include "Game.h"
#	include "Player.h"

#	include "BinParser.h"

#	include "NodeManager.h"

#	include "Consts.h"

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
		Scene * scene = Player::get()
			->getCurrentScene();

		return scene;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Arrow::_compile()
	{
		bool cursorMode = 
			Application::get()->getCursorMode();

		setCursorMode( cursorMode );

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

		InputEngine::get()
			->addCursorPositionProvider(this);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::_deactivate()
	{	
		InputEngine::get()
			->removeCursorPositionProvider(this);

		Entity::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::onCursorPositionChange( const mt::vec2f & _position )
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
	void Arrow::setCursorMode( bool _mode )
	{
		if( m_child.empty() )
		{
			return;
		}

		ConstString c_CursorNode("CursorNode");
		Node* mainCursor = this->findChildren( c_CursorNode, false );

		if( mainCursor == NULL )
		{
			return;
		}
		
		if( _mode )
		{
			mainCursor->disable();
		}
		else
		{
			mainCursor->enable();
		}
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

		MENGE_LOG_ERROR("resolutionScale %f %f", scale.x, scale.y);

		mt::scale_m3( m_clickMatrix, scale );
	}
	//////////////////////////////////////////////////////////////////////////
	void Arrow::invalidateClickMatrix_()
	{
		m_invalidateClickMatrix = true;
	}
}
