#	include "Sprite.h" 

#	include "Layer2D.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"

#	include "RenderEngine.h"

#	include "ResourceImage.h"

#	include "ResourceManager.h"

#	include "LogEngine.h"

#	include "math/box2.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(Sprite);
	//////////////////////////////////////////////////////////////////////////
	Sprite::Sprite()
	: m_resource( 0 )
	, m_color( 1.0f, 1.0f, 1.0f, 1.0f )
	, m_currentImageIndex( 0 )
	, m_centerAlign( false )
	, m_alignOffset( 0.f, 0.f )
	, m_scale( 1.0f, 1.0f )
	, m_percent( 0.0f, 0.0f, 0.0f, 0.0f )
	, m_uv( 0.0f, 0.0f, 1.0f, 1.0f )
	, m_offset( 0.0f, 0.0f )
	, m_size( 0.0f, 0.0f )
	, m_changingColorTime( 0.0f )
	, m_changingColor( false )
	, m_flipX( false )
	, m_flipY( false )
	, m_newColor( 1.0f, 1.0f, 1.0f, 1.0f )
	, m_blendSrc( BF_SOURCE_ALPHA )
	, m_blendDest( BF_ONE_MINUS_SOURCE_ALPHA )
	{ }
	//////////////////////////////////////////////////////////////////////////
	Sprite::~Sprite()
	{}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::loader( XmlElement * _xml )
	{
		SceneNode2D::loader(_xml);

		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE( "ImageMap", "Name", m_resourcename );
			XML_CASE_ATTRIBUTE_NODE( "ImageIndex", "Value", m_currentImageIndex );
			XML_CASE_ATTRIBUTE_NODE( "CenterAlign", "Value", m_centerAlign );
			XML_CASE_ATTRIBUTE_NODE( "Scale", "Value", m_scale );
			XML_CASE_ATTRIBUTE_NODE( "Blend", "Source", ((int&)m_blendSrc) );
			XML_CASE_ATTRIBUTE_NODE( "Blend", "Dest", ((int&)m_blendDest) );
			XML_CASE_ATTRIBUTE_NODE( "Color", "Value", m_color );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Sprite::_activate()
	{
		if( SceneNode2D::_activate() == false )
		{
			return false;
		}

		updateAlign_();

		//this->registerEventMethod("COLOR_END", "onColorEnd" );
		//this->registerEventMethod("COLOR_STOP", "onColorStop" );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_deactivate()
	{
		SceneNode2D::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Sprite::_compile()
	{
		if( SceneNode2D::_compile() == false )
		{
			return false;
		}

		if( m_resourcename != "" )
		{
			m_resource = 
				Holder<ResourceManager>::hostage()
				->getResourceT<ResourceImage>( m_resourcename );

			if( m_resource == 0 )
			{
				MENGE_LOG( "Image resource not getting '%s'", m_resourcename.c_str() );
				return false;
			}
		}
		else
			return false;

		updateAlphaBlend_();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_release()
	{
		SceneNode2D::_release();

		Holder<ResourceManager>::hostage()
			->releaseResource( m_resource );

		m_resource = 0;
	}
	///////////////////////////////////////////////////////////////////////////
	void Sprite::setScale( const mt::vec2f& _scale )
	{
		m_scale = _scale;

		if( m_active )
		{
			updateAlign_();
		}
	}
	///////////////////////////////////////////////////////////////////////////
	const mt::vec2f& Sprite::getScale() const
	{
		return m_scale;
	}
	///////////////////////////////////////////////////////////////////////////
	void Sprite::setPercentVisibility( const mt::vec2f & _percentX, const mt::vec2f & _percentY )
	{
		m_percent.v2_0 = _percentX;
		m_percent.v2_1 = _percentY;
	}
	///////////////////////////////////////////////////////////////////////////
	bool Sprite::isVisible( const Viewport & _viewPort )
	{
		if( m_resource == NULL )
		{
			MENGE_LOG( "Sprite %s: Image resource not getting '%s'"
				, getName().c_str()
				, m_resourcename.c_str() 
				);
		}

		/*Max*/
		m_size = m_resource->getSize( m_currentImageIndex );
	
		m_size.x *= m_scale.x;
		m_size.y *= m_scale.y;

		mt::vec2f offset = m_resource->getOffset( m_currentImageIndex );
		if( m_flipX )
		{
			offset.x = m_resource->getMaxSize( m_currentImageIndex ).x - ( m_resource->getSize( m_currentImageIndex ).x + offset.x );
		}
		if( m_flipY )
		{
			offset.y = m_resource->getMaxSize( m_currentImageIndex ).y - ( m_resource->getSize( m_currentImageIndex ).y + offset.y );
		}

		m_offset = offset + m_alignOffset;

		mt::mat3f wm = getWorldMatrix();

		if( m_layer && m_layer->isScrollable() )
		{
			float c = ::floorf( wm.v2.x / m_layer->getSize().x );
			wm.v2.x -= m_layer->getSize().x * c;
		}

		mt::box2f bbox;

		mt::set_box_from_oriented_extent( bbox, m_offset, m_size, wm );

		bool result = _viewPort.testRectangle( bbox.min, bbox.max );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setImageIndex( unsigned int _index )
	{
		m_currentImageIndex = _index;
		updateAlphaBlend_();
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int Sprite::getImageIndex() const
	{
		return m_currentImageIndex;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setImageResource( const std::string & _name )
	{
		if( m_resourcename != _name )
		{
			m_resourcename = _name;

			recompile();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const std::string & Sprite::getImageResource() const
	{
		return m_resourcename;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::updateAlign_()
	{
		if( m_centerAlign )
		{
			if( m_resource == NULL )
			{
				MENGE_LOG( "Sprite %s: Image resource not getting '%s'"
					, getName().c_str()
					, m_resourcename.c_str() 
					);
			}

			mt::vec2f size = m_resource->getMaxSize( 0 );
		
			size.x *= m_scale.x;
			size.y *= m_scale.y;

			m_alignOffset = size * -0.5f;
			m_alignOffset.x = ::floorf( m_alignOffset.x + 0.5f );
			m_alignOffset.y = ::floorf( m_alignOffset.y + 0.5f );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::flip( bool _x )
	{
		if( _x )
		{
			m_flipX = !m_flipX;
		}
		else
		{
			m_flipY = !m_flipY;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::updateAlphaBlend_()
	{
		m_blendSrc = BF_SOURCE_ALPHA;
		m_blendDest = BF_ONE_MINUS_SOURCE_ALPHA;

		if(m_resource != NULL)
		{
			bool isAlpha = m_resource->isAlpha(m_currentImageIndex);

			if(isAlpha == false)
			{
			//	m_blendSrc = BF_ONE; // не хочет так работать
			//	m_blendDest = BF_ZERO;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::updateVisibility_()
	{	
		if( m_resource == NULL )
		{
			MENGE_LOG( "Sprite %s: Image resource not getting '%s'"
				, getName().c_str()
				, m_resourcename.c_str() 
				);
		}

		m_uv = m_resource->getUV( m_currentImageIndex );

		m_offset.x += m_size.x * m_percent.x;
		m_offset.y += m_size.y * m_percent.y;

        m_size.x *= ( 1.0f - m_percent.x );
        m_size.x *= ( 1.0f - m_percent.z );

        m_size.y *= ( 1.0f - m_percent.y );
        m_size.y *= ( 1.0f - m_percent.w );
		
		m_uv.x = m_uv.x * ( 1.0f - m_percent.x ) + m_percent.x * m_uv.z; 
        m_uv.y = m_uv.y * ( 1.0f - m_percent.y ) + m_percent.y * m_uv.w;

		m_uv.z = m_uv.x * m_percent.z + ( 1.0f - m_percent.z ) * m_uv.z; 
		m_uv.w = m_uv.y * m_percent.w + ( 1.0f - m_percent.w ) * m_uv.w; 
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::updateFlip_()
	{	
		if( m_flipX == true )
		{
			std::swap( m_uv.x, m_uv.z );
		}

		if( m_flipY == true )
		{
			std::swap( m_uv.y, m_uv.w );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::colorTo( const Color & _color, float _time )
	{
		m_newColor = _color;
		m_changingColorTime = _time;
		if( m_changingColor )
		{
			this->callEvent( "COLOR_STOP", "(O)", this->getEmbedding() );
		}
		m_changingColor = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setAlpha( float _alpha )
	{
		m_color.a = _alpha;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::alphaTo( float _alpha, float _time )
	{
		m_newColor = m_color;
		m_newColor.a = _alpha;
		m_changingColorTime = _time;
		if( m_changingColor )
		{
			this->callEvent( "COLOR_STOP", "(O)", this->getEmbedding() );
		}
		m_changingColor = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_render( bool _enableDebug )
	{
		updateVisibility_();

		updateFlip_();

		if( m_resource == NULL )
		{
			MENGE_LOG( "Sprite %s: Image resource not getting '%s'"
				, getName().c_str()
				, m_resourcename.c_str() 
				);
		}

		const RenderImageInterface * renderImage = m_resource->getImage( m_currentImageIndex );

		//const mt::mat3f & wm = getWorldMatrix();

		mt::mat3f wm = getWorldMatrix();
		if( m_layer && m_layer->isScrollable() )
		{
			float c = ::floorf( wm.v2.x / m_layer->getSize().x );
			wm.v2.x -= m_layer->getSize().x * c;
		}

		Holder<RenderEngine>::hostage()->renderImage(
			wm,
			m_offset,
			m_offset + mt::vec2f( m_size.x, 0.0f ),
			m_offset + m_size,
			m_offset + mt::vec2f( 0.0f, m_size.y ),
			m_uv,
			m_color.get(),
			renderImage,
			m_blendSrc,
			m_blendDest 
			);
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setColor( const Color & _color )
	{
		m_color = _color;
	}
	//////////////////////////////////////////////////////////////////////////
	const Color & Sprite::getColor() const
	{
		return m_color;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_update( float _timing )
	{
		if( m_changingColor )
		{
			if( m_changingColorTime < _timing )
			{
				m_color = m_newColor;
				m_changingColor = false;
				m_changingColorTime = 0.f;	
				this->callEvent( "COLOR_END", "(O)", this->getEmbedding() );
			}
			else
			{
				float d = _timing / m_changingColorTime;
				m_color = m_newColor * d + m_color * ( 1.0f - d );
				m_changingColorTime -= _timing;
			}

		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setListener( PyObject* _listener )
	{
		//m_listener = _listener;
		//SceneNode2D::_onSetListener();
		SceneNode2D::setListener( _listener );

		registerEventListener("COLOR_END", "onColorEnd", _listener );
		registerEventListener("COLOR_STOP", "onColorStop", _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f Sprite::getImageSize()
	{
		if( m_resource == NULL )
		{
			MENGE_LOG( "Sprite %s: Image resource not getting '%s'"
				, getName().c_str()
				, m_resourcename.c_str() 
				);
		}

		return m_resource->getSize( m_currentImageIndex );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Sprite::getCenterAlign()
	{
		return m_centerAlign;
	}
	//////////////////////////////////////////////////////////////////////////
}