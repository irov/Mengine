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
	, m_scale( 1.0f )
	, m_percent( 0.0f, 0.0f, 0.0f, 0.0f )
	{}
	//////////////////////////////////////////////////////////////////////////
	Sprite::~Sprite()
	{}
	///////////////////////////////////////////////////////////////////////////
	void Sprite::setScale( float _scale )
	{
		m_scale = _scale;

		updateAlign_();
	}
	///////////////////////////////////////////////////////////////////////////
	float Sprite::getScale() const
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
		mt::vec2f size = m_resource->getMaxSize( m_currentImageIndex );
		size *= m_scale;

		const mt::vec2f & image_offset = m_resource->getOffset( m_currentImageIndex );
		mt::vec2f offset = m_centerAlign ? image_offset + m_alignOffset : image_offset;

		const mt::mat3f & worldMatrix = getWorldMatrix();

		mt::box2f bbox;

		mt::set_box_from_oriented_extent( bbox, offset, size, worldMatrix );

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
	//////////////////////////////////////////////////////////////////////////
	void Sprite::updateAlign_()
	{
		if( m_centerAlign )
		{
			mt::vec2f size = m_resource->getMaxSize( 0 );
			size *= m_scale;

			m_alignOffset = size * (- 0.5f);
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
	void Sprite::_render()
	{
		mt::vec2f size = m_resource->getSize( m_currentImageIndex );
		size *= m_scale;

		mt::vec2f image_offset = m_resource->getOffset( m_currentImageIndex );
		mt::vec4f frame_uv = m_resource->getUV( m_currentImageIndex );

		image_offset.x += size.x * m_percent.x;
        size.x *= ( 1.0f - m_percent.x );
        size.x *= ( 1.0f - m_percent.z );
     
        image_offset.y += size.y * m_percent.y;
        size.y *= ( 1.0f - m_percent.y );
        size.y *= ( 1.0f - m_percent.w );

		frame_uv.x = frame_uv.x * ( 1.0f - m_percent.x ) + m_percent.x * frame_uv.z; 
        frame_uv.y = frame_uv.y * ( 1.0f - m_percent.y ) + m_percent.y * frame_uv.w;

		frame_uv.z *= (1.0f - m_percent.z);
		frame_uv.w *= (1.0f - m_percent.w);

		mt::vec2f offset = m_centerAlign ? image_offset + m_alignOffset : image_offset;

		const RenderImageInterface * renderImage = m_resource->getImage( m_currentImageIndex );

		const mt::mat3f & rwm = getWorldMatrix();

		Holder<RenderEngine>::hostage()->renderImage(
			rwm, 
			offset,
			frame_uv,
			size,
			m_color,
			renderImage
			);
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


