#	include "Sprite.h" 

#	include "ObjectImplement.h"

#	include "RenderEngine.h"
#	include "Viewport.h"

#	include "XmlParser.h"

#	include "RenderSystemInterface.h"

#	include "FileSystemInterface.h"

#	include "ResourceImage.h"

#	include "ResourceManager.h"

#	include "math/bv.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(Sprite);
//////////////////////////////////////////////////////////////////////////
Sprite::Sprite()
: m_image(0)
, m_offset(0.f,0.f)
, m_color(0xFFFFFFFF)
, m_currentImageIndex(0) 
{

}
//////////////////////////////////////////////////////////////////////////
Sprite::~Sprite()
{
}
///////////////////////////////////////////////////////////////////////////
bool Sprite::isVisible(const Viewport & _viewPort)
{
	const mt::vec2f & pos = getWorldPosition();
	const mt::mat3f & wm = getWorldMatrix();

	mt::vec3f min0, max0;

	const mt::vec2f & size = m_image->getMaxSize();

	const mt::vec2f & localPosition = getLocalPosition();
	const mt::mat3f & worldMatrix = getWorldMatrix();

	calculate_aabb_from_obb(
		min0,
		max0,
		localPosition+m_offset,
		size,
		worldMatrix);

	if (max0.x < _viewPort.begin.x || min0.x > _viewPort.end.x ) return false;
	if (max0.y < _viewPort.begin.y || min0.y > _viewPort.end.y ) return false;

	Holder<RenderEngine>::hostage()->drawLine(min0.v2,max0.v2,2,0xffff00ff);

	return true;
}
//////////////////////////////////////////////////////////////////////////
void Sprite::setImageIndex( size_t _index )
{
	size_t size = m_image->getCount();

	if(_index >= size)
	{
		assert(!"You can't set image index > image size");
	}

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
	m_resourceName = _name;
}
//////////////////////////////////////////////////////////////////////////
const std::string & Sprite::getImageResource() const
{
	return m_resourceName;
}
//////////////////////////////////////////////////////////////////////////
void Sprite::setOffset(const mt::vec2f& _offset)
{
	m_offset = _offset;
}
//////////////////////////////////////////////////////////////////////////
bool Sprite::_activate()
{
	m_image = 
		Holder<ResourceManager>::hostage()
		->getResourceT<ResourceImage>( m_resourceName );

	if( m_image == 0 )
	{
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
void Sprite::_deactivate()
{
	Holder<ResourceManager>::hostage()
		->releaseResource( m_image );
}
//////////////////////////////////////////////////////////////////////////
void Sprite::loader(TiXmlElement *xml)
{
	XML_FOR_EACH_TREE(xml)
	{
		XML_CHECK_VALUE_NODE( "ImageMap", "Name", m_resourceName );
	}

	SceneNode2D::loader(xml);
}
//////////////////////////////////////////////////////////////////////////
void Sprite::_render( const mt::mat3f &rwm, const Viewport & _viewPort )
{
	const mt::vec2f & size = m_image->getSize( m_currentImageIndex );
	const mt::vec2f & image_offset = m_image->getOffset( m_currentImageIndex );
	const mt::vec4f & frame_uv = m_image->getUV( m_currentImageIndex );
	
	RenderImageInterface * renderImage = m_image->getImage( m_currentImageIndex );

	Holder<RenderEngine>::hostage()->renderImage(
		rwm, 
		image_offset + m_offset,
		frame_uv,
		size,
		m_color,
		renderImage
		);
}
//////////////////////////////////////////////////////////////////////////
void Sprite::_debugRender()
{};



