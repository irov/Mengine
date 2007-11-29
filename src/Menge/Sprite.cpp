#	include "Sprite.h" 

#	include "ObjectImplement.h"

#	include "RenderEngine.h"

#	include "Viewport.h"

#	include "XmlParser/XmlParser.h"

#	include "Interface/RenderSystemInterface.h"

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
	, m_color( 0xFFFFFFFF )
	, m_currentImageIndex( 0 )
	, m_centerAlign( false )
	, m_alignOffset( 0.f, 0.f )
	, m_scale( 1.0f, 1.0f )
	, m_percent( 0.0f, 0.0f, 0.0f, 0.0f )
	, m_changingColorTime( 0.0f )
	, m_flipX( false )
	, m_flipY( false )
	{}
	//////////////////////////////////////////////////////////////////////////
	Sprite::~Sprite()
	{}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::loader( TiXmlElement * _xml )
	{
		SceneNode2D::loader(_xml);

		XML_FOR_EACH_TREE(_xml)
		{
			XML_CHECK_VALUE_NODE( "ImageMap", "Name", m_resourcename );
			XML_CHECK_VALUE_NODE( "ImageIndex", "Value", m_currentImageIndex );
			XML_CHECK_VALUE_NODE( "CenterAlign", "Value", m_centerAlign );
			XML_CHECK_VALUE_NODE( "Scale", "Value", m_scale );
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

		m_resource = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceImage>( m_resourcename );

		if( m_resource == 0 )
		{
			MENGE_LOG( "Image resource loading failed '%s'\n", m_resourcename.c_str() );
			return false;
		}

		return true;		
	}
	///////////////////////////////////////////////////////////////////////////
	void Sprite::setScale( mt::vec2f _scale )
	{
		m_scale = _scale;

		updateAlign_();
	}
	///////////////////////////////////////////////////////////////////////////
	mt::vec2f Sprite::getScale() const
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
		m_size = m_resource->getMaxSize( m_currentImageIndex );
	
		m_size.x *= m_scale.x;
		m_size.y *= m_scale.y;

		const mt::vec2f & offset = m_resource->getOffset( m_currentImageIndex );
		m_offset = offset + m_alignOffset;

		const mt::mat3f & wm = getWorldMatrix();

		mt::box2f bbox;

		mt::set_box_from_oriented_extent( bbox, m_offset, m_size, wm );

		bool result = _viewPort.testRectangle( bbox.min, bbox.max );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setImageIndex( size_t _index )
	{
		m_currentImageIndex = _index;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Sprite::getImageIndex() const
	{
		return m_currentImageIndex;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::setImageResource( const std::string & _name )
	{
		m_resourcename = _name;

		if( m_resource )
		{
			Holder<ResourceManager>::hostage()
				->releaseResource( m_resource );
		}
		
		m_resource = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceImage>( m_resourcename );
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
			mt::vec2f size = m_resource->getMaxSize( 0 );
		
			size.x *= m_scale.x;
			size.y *= m_scale.y;

			m_alignOffset = size * -0.5f;
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
	void Sprite::_release()
	{
		SceneNode2D::_release();

		Holder<ResourceManager>::hostage()
			->releaseResource( m_resource );

		m_resource = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::updateVisibility_()
	{	
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
	void Sprite::setColorTime( unsigned int _color, float _time )
	{
		m_newColor = _color;
		m_changingColorTime = _time;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_render()
	{
		updateVisibility_();

		updateFlip_();

		const RenderImageInterface * renderImage = m_resource->getImage( m_currentImageIndex );

		const mt::mat3f & wm = getWorldMatrix();

		Holder<RenderEngine>::hostage()->renderImage(
			wm, 
			m_offset,
			m_uv,
			m_size,
			m_color,
			renderImage
			);
	}

	void Sprite::_update( float _timing )
	{
		if( m_changingColorTime > 0.0f )
		{
			float d = _timing / m_changingColorTime;
			m_color = m_newColor * d + ( 1.0f - d ) * m_color;
			m_changingColorTime -= _timing;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void Sprite::_debugRender()
	{
		//const mt::vec2f & pos = getWorldPosition();
		//const mt::vec2f & size = m_image->getMaxSize();
		//Holder<RenderEngine>::hostage()
		//	->drawLine( pos, pos + size, 2, 0xffff00ff );
	}
}


