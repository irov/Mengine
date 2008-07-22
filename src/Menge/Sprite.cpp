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
	//, m_changingColorTime( 0.0f )
	//, m_changingColor( false )
	, m_flipX( false )
	, m_flipY( false )
	//, m_newColor( 1.0f, 1.0f, 1.0f, 1.0f )
	, m_blendSrc( BF_SOURCE_ALPHA )
	, m_blendDest( BF_ONE_MINUS_SOURCE_ALPHA )
	{ }
	//////////////////////////////////////////////////////////////////////////
	Sprite::~Sprite()
	{}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::loader( XmlElement * _xml )
	{
		Node::loader(_xml);

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
		if( Node::_activate() == false )
		{
			return false;
		}

		updateSprite_();

		//this->registerEventMethod("COLOR_END", "onColorEnd" );
		//this->registerEventMethod("COLOR_STOP", "onColorStop" );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_deactivate()
	{
		Node::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Sprite::_compile()
	{
		if( Node::_compile() == false )
		{
			return false;
		}

		if( m_resourcename.empty() == false )
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
		Node::_release();

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
			updateSprite_();
		}
	}
	///////////////////////////////////////////////////////////////////////////
	const mt::vec2f& Sprite::getScale() const
	{
		return m_scale;
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
		updateSprite_();
	}
	///////////////////////////////////////////////////////////////////////////
	void Sprite::setPercentVisibility( const mt::vec2f & _percentX, const mt::vec2f & _percentY )
	{
		m_percent.v2_0 = _percentX;
		m_percent.v2_1 = _percentY;
		updateSprite_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setImageIndex( unsigned int _index )
	{
		m_currentImageIndex = _index;
		updateSprite_();
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
	void Sprite::updateAlphaBlend_()
	{
		m_blendSrc = BF_SOURCE_ALPHA;
		m_blendDest = BF_ONE_MINUS_SOURCE_ALPHA;

		if(m_resource != NULL)
		{
			bool isAlpha = m_resource->isAlpha(m_currentImageIndex);

			if(isAlpha == false)
			{
				m_blendSrc = BF_ONE; // �� ����� ��� ��������
				m_blendDest = BF_ZERO;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::updateSprite_()
	{
		if( m_resource == 0 ) return;

		m_size = m_resource->getSize( m_currentImageIndex );

		m_size.x *= m_scale.x;
		m_size.y *= m_scale.y;

		m_size.x *= ( 1.0f - m_percent.x );
		m_size.x *= ( 1.0f - m_percent.z );

		m_size.y *= ( 1.0f - m_percent.y );
		m_size.y *= ( 1.0f - m_percent.w );

		if( m_centerAlign )
		{
			mt::vec2f size = m_resource->getMaxSize( 0 );

			size.x *= m_scale.x;
			size.y *= m_scale.y;

			m_alignOffset = size * -0.5f;
		}

		mt::vec2f offset = m_resource->getOffset( m_currentImageIndex );
		const mt::vec2f & maxSize = m_resource->getMaxSize( m_currentImageIndex );
		const mt::vec2f & size = m_resource->getSize( m_currentImageIndex );

		if( m_flipX )
		{
			offset.x = maxSize.x - ( size.x + offset.x );
		}

		if( m_flipY )
		{
			offset.y = maxSize.y - ( size.y + offset.y );
		}

		offset.x *= m_scale.x;
		offset.y *= m_scale.y;

		m_offset = offset + m_alignOffset;

		m_uv = m_resource->getUV( m_currentImageIndex );

		m_uv.x = m_uv.x * ( 1.0f - m_percent.x ) + m_percent.x * m_uv.z; 
		m_uv.y = m_uv.y * ( 1.0f - m_percent.y ) + m_percent.y * m_uv.w;

		m_uv.z = m_uv.x * m_percent.z + ( 1.0f - m_percent.z ) * m_uv.z; 
		m_uv.w = m_uv.y * m_percent.w + ( 1.0f - m_percent.w ) * m_uv.w;

		if( m_flipX == true )
		{
			std::swap( m_uv.x, m_uv.z );
		}

		if( m_flipY == true )
		{
			std::swap( m_uv.y, m_uv.w );
		}

		setLocalBoundingBox( mt::box2f( m_offset, m_offset + m_size ) );
		updateBoundingBox();
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::colorTo( const ColourValue & _color, float _time )
	{
		if( m_colorTo.isStarted() )
		{
			this->callEvent( "COLOR_STOP", "(O)", this->getEmbedding() );
			m_colorTo.stop();
		}
		if( m_colorTo.start( m_color, _color, _time, length_color ) == false )
		{
			m_color = _color;
			callEvent( "COLOR_END", "(O)", this->getEmbedding() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setAlpha( float _alpha )
	{
		m_color.a = _alpha;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::alphaTo( float _alpha, float _time )
	{
		if( m_colorTo.isStarted() )
		{
			this->callEvent( "COLOR_STOP", "(O)", this->getEmbedding() );
			m_colorTo.stop();
		}
		ColourValue newColor = m_color;
		newColor.a = _alpha;
		if( m_colorTo.start( m_color, newColor, _time, length_color ) == false )
		{
			m_color	 = newColor;
			callEvent( "COLOR_END", "(O)", this->getEmbedding() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_render( const Viewport & _viewport, bool _enableDebug )
	{
		if( m_resource == NULL )
		{
			MENGE_LOG( "Sprite %s: Image resource not getting '%s'"
				, getName().c_str()
				, m_resourcename.c_str() 
				);
		}

		const RenderImageInterface * renderImage = m_resource->getImage( m_currentImageIndex );

		const mt::mat3f & wm = getWorldMatrix();
	
		Holder<RenderEngine>::hostage()->renderImage(
			wm,
			m_offset,
			m_offset + mt::vec2f( m_size.x, 0.0f ),
			m_offset + m_size,
			m_offset + mt::vec2f( 0.0f, m_size.y ),
			m_uv,
			m_color.getAsARGB(),
			renderImage,
			m_blendSrc,
			m_blendDest 
			);
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setColor( const ColourValue & _color )
	{
		m_color = _color;
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue & Sprite::getColor() const
	{
		return m_color;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_update( float _timing )
	{
		/*if( m_changingColor )
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

		}*/
		if( m_colorTo.isStarted() )
		{
			if( m_colorTo.update( _timing, &m_color ) == true )
			{
				this->callEvent( "COLOR_END", "(O)", this->getEmbedding() );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_setListener()
	{
		this->registerEvent("COLOR_END", "onColorEnd", m_listener );
		this->registerEvent("COLOR_STOP", "onColorStop", m_listener );
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f Sprite::getImageSize()
	{
		if( m_resource == NULL )
		{
			MENGE_LOG( "Sprite %s: Can't get image size, because resource is NULL '%s'"
				, getName().c_str()
				, m_resourcename.c_str() 
				);

			return mt::vec2f(0.f,0.f);
		}

		const mt::vec2f & size = 
			m_resource->getSize( m_currentImageIndex );

		return size;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Sprite::getCenterAlign()
	{
		return m_centerAlign;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::colorToStop()
	{
		m_colorTo.stop();
		callEvent( "COLOR_STOP", "(O)", this->getEmbedding() );
	}
	//////////////////////////////////////////////////////////////////////////
}