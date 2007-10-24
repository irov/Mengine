#	include "Sprite.h" 

#	include "ObjectImplement.h"

#	include "RenderEngine.h"

#	include "Viewport.h"

#	include "XmlParser/XmlParser.h"

#	include "Interface/RenderSystemInterface.h"

#	include "ResourceImage.h"

#	include "ResourceManager.h"

#	include "math/box2.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(Sprite);
	//////////////////////////////////////////////////////////////////////////
	Sprite::Sprite()
	: m_image(0)
	, m_color(0xFFFFFFFF)
	, m_currentImageIndex(0) 
	{}
	//////////////////////////////////////////////////////////////////////////
	Sprite::~Sprite()
	{}
	///////////////////////////////////////////////////////////////////////////
	bool Sprite::isVisible( const Viewport & _viewPort )
	{
		const mt::vec2f & size = m_image->getMaxSize( m_currentImageIndex );
		const mt::vec2f & offset = m_image->getOffset( m_currentImageIndex );
		const mt::mat3f & worldMatrix = getWorldMatrix();

		mt::box2f	bbox;

		mt::set_box_from_oriented_extent( bbox, size + offset, worldMatrix );
		return _viewPort.testRectangle( bbox.min, bbox.max );
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
		m_resourceImage = _name;

		if( m_image )
		{
			Holder<ResourceManager>::hostage()
				->releaseResource( m_image );
		}
		
		m_image = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceImage>( m_resourceImage );
	}
	//////////////////////////////////////////////////////////////////////////
	const std::string & Sprite::getImageResource() const
	{
		return m_resourceImage;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Sprite::_activate()
	{
		if( SceneNode2D::_activate() == false )
		{
			return false;
		}

		m_image = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceImage>( m_resourceImage );

		if( m_image == 0 )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_deactivate()
	{
		SceneNode2D::_deactivate();

		Holder<ResourceManager>::hostage()
			->releaseResource( m_image );

		m_image = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::loader(TiXmlElement * _xml)
	{
		SceneNode2D::loader(_xml);

		XML_FOR_EACH_TREE(_xml)
		{
			XML_CHECK_VALUE_NODE( "ImageMap", "Name", m_resourceImage );
			XML_CHECK_VALUE_NODE( "ImageIndex", "Value", m_currentImageIndex);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_render( const mt::mat3f & _rwm, const Viewport & _viewPort )
	{
		const mt::vec2f & size = m_image->getSize( m_currentImageIndex );
		const mt::vec2f & image_offset = m_image->getOffset( m_currentImageIndex );
		const mt::vec4f & frame_uv = m_image->getUV( m_currentImageIndex );
		
		const RenderImageInterface * renderImage = m_image->getImage( m_currentImageIndex );

		Holder<RenderEngine>::hostage()->renderImage(
			_rwm, 
			image_offset,
			frame_uv,
			size,
			m_color,
			renderImage
			);
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_debugRender()
	{
	/*	const mt::vec2f & pos = getWorldPosition();
		const mt::vec2f & size = m_image->getMaxSize();
		Holder<RenderEngine>::hostage()
			->drawLine( pos, pos + size, 2, 0xffff00ff );*/
	}
}


